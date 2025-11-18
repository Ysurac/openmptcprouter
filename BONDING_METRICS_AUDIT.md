# BONDING METRICS AUDIT REPORT
**Date:** 2025-11-18
**Focus:** Are link metrics USED in bonding decisions or just collected/logged?

## EXECUTIVE SUMMARY

**Critical Finding:** There is a MAJOR DISCONNECT between metrics collection and usage:

1. ✅ **MPTCP Kernel Schedulers DO use RTT** - confirmed in code
2. ❌ **UCI metrics are NOT used by MPTCP** - they're for routing tables only
3. ❌ **User-space metrics (signal, latency, throughput) are NOT used for decisions** - display only
4. ✅ **BBR uses metrics internally** - but only for rate control, not path selection

**Verdict:** The system IS adaptive at the kernel level (RTT-based), but user-space monitoring is purely informational.

---

## 1. MPTCP Schedulers (Kernel 5.4 Legacy)

### Finding: RTT IS ACTIVELY USED ✅

**Location:** `/5.4/target/linux/generic/hack-5.4/690-mptcp_v0.96.patch`

#### Evidence 1: Path Selection Uses Minimum RTT

**File:** `mptcp_sched.c` lines 24043-24046

```c
if (tp->srtt_us < min_srtt) {
    min_srtt = tp->srtt_us;
    bestsk = sk;
}
```

**Function:** `get_subflow_from_selectors()`

**Behavior:** 
- Iterates through all available subflows
- Compares `srtt_us` (smoothed RTT in microseconds)
- Selects subflow with LOWEST RTT
- This executes in the packet scheduling hot path

**Conclusion:** ✅ RTT values ARE READ and USED to select which path gets packets.

---

#### Evidence 2: Slow Path Penalization

**File:** `mptcp_sched.c` lines 24160-24167

```c
if (tp->srtt_us < tp_it->srtt_us && 
    inet_csk((struct sock *)tp_it)->icsk_ca_state == TCP_CA_Open) {
    u32 prior_cwnd = tp_it->snd_cwnd;
    
    tp_it->snd_cwnd = max(tp_it->snd_cwnd >> 1U, 1U);
    
    /* If in slow start, do not reduce the ssthresh */
    if (prior_cwnd >= tp_it->snd_ssthresh)
        tp_it->snd_ssthresh = max(tp_it->snd_ssthresh >> 1U, 2U);
```

**Function:** `mptcp_rcv_buf_optimization()`

**Behavior:**
- Compares RTT of current subflow vs other subflows
- If current subflow is faster (lower RTT), it **halves the congestion window** of slower flows
- This actively penalizes high-latency paths
- Runs during receive buffer optimization

**Conclusion:** ✅ RTT is used to make ACTIVE decisions that affect throughput distribution.

---

#### Evidence 3: Retransmission Path Selection

**File:** `mptcp_sched.c` lines 24189-24195

```c
if (4 * tp->srtt_us >= tp_it->srtt_us) {
    do_retrans = false;
    break;
} else {
    do_retrans = true;
}
```

**Function:** `mptcp_rcv_buf_optimization()`

**Behavior:**
- Compares RTT × 4 of fast path vs slow path
- Decides whether to retransmit lost packets on a different (faster) path
- If fast path is significantly faster, triggers retransmission

**Conclusion:** ✅ RTT directly controls retransmission strategy.

---

#### Evidence 4: Scheduler Description

**File:** `mptcp_sched.c` lines 24070-24076

```c
/* This is the scheduler. This function decides on which flow to send
 * a given MSS. If all subflows are found to be busy, NULL is returned
 * The flow is selected based on the shortest RTT.
 * If all paths have full cong windows, we simply return NULL.
 *
 * Additionally, this function is aware of the backup-subflows.
 */
```

**Conclusion:** ✅ Documentation confirms RTT-based scheduling is intentional.

---

### What About Loss/Throughput?

**Searched for:** Loss rate, throughput metrics in scheduler

**Result:** ❌ NOT FOUND in default scheduler

**Explanation:**
- Default scheduler uses **RTT only**
- Loss is handled by congestion control (BBR), not scheduler
- Throughput is implicit (fast paths = higher throughput)

---

### Other Schedulers Available

**ECF (Earliest Completion First) Scheduler:**

**File:** `mptcp_ecf.c` lines 13395-13398, 13412-13417

```c
/* record minimal rtt */
if (besttp->srtt_us < min_srtt) {
    min_srtt = besttp->srtt_us;
    minsk = bestsk;
}

// Later uses:
u32 cwnd_f = tcp_sk(minsk)->snd_cwnd;
u32 srtt_f = tcp_sk(minsk)->srtt_us >> 3;
u32 rttvar_f = tcp_sk(minsk)->rttvar_us >> 1;  // RTT variance

u32 cwnd_s = tcp_sk(bestsk)->snd_cwnd;
u32 srtt_s = tcp_sk(bestsk)->srtt_us >> 3;
u32 rttvar_s = tcp_sk(bestsk)->rttvar_us >> 1;
```

**Behavior:**
- Uses RTT + RTT variance + congestion window
- Estimates completion time for each path
- Selects path with earliest estimated completion
- **More sophisticated than default, but still RTT-based**

**Conclusion:** ✅ Advanced schedulers also rely on RTT, not loss/signal metrics.

---

## 2. MPTCP Schedulers (Kernel 6.12 Mainline)

### Finding: BPF Infrastructure with Limited Metric Exposure

**Location:** `/6.12/target/linux/generic/hack-6.12/999-mptcp-bpf.patch`

#### Evidence 1: BPF Struct Access

**File:** `bpf.c` lines 303-304

```c
case offsetof(struct mptcp_subflow_context, avg_pacing_rate):
    end = offsetofend(struct mptcp_subflow_context, avg_pacing_rate);
    break;
```

**Behavior:**
- BPF programs can READ/WRITE `avg_pacing_rate`
- This is a throughput-related metric
- Allows custom schedulers to consider pacing rate

**Conclusion:** ✅ BPF programs CAN access performance metrics.

---

#### Evidence 2: Exported Functions

**File:** `bpf.c` lines 534-539

```c
BTF_ID_FLAGS(func, mptcp_subflow_active)
BTF_ID_FLAGS(func, mptcp_set_timeout)
BTF_ID_FLAGS(func, mptcp_wnd_end)
BTF_ID_FLAGS(func, tcp_stream_memory_free)
BTF_ID_FLAGS(func, bpf_mptcp_subflow_queues_empty)
BTF_ID_FLAGS(func, mptcp_pm_subflow_chk_stale)
```

**Behavior:**
- BPF programs can call these helper functions
- Can check if subflow is active, stale, has queued data
- Can check memory/buffer status

**Limitation:** ❌ RTT/loss/signal NOT directly exposed

**Conclusion:** ⚠️ BPF schedulers have LIMITED access to performance metrics.

---

#### Evidence 3: Default Scheduler

**File:** `sched.c` lines 204-205

```c
if (msk->sched == &mptcp_sched_default || !msk->sched)
    return mptcp_sched_default_get_subflow(msk, &data);
```

**Behavior:**
- References default scheduler (implementation in mainline kernel, not patch)
- Likely similar to 5.4 RTT-based logic

**Conclusion:** ⚠️ Default behavior unclear without mainline source.

---

## 3. UCI "metric" Parameter

### Finding: NOT Used by MPTCP ❌

**Location:** `/common/files/usr/bin/port-autoconfig.sh` line 141

```bash
uci -q batch <<-'EOF'
    delete network.wan
    set network.wan=interface
    set network.wan.proto='dhcp'
    set network.wan.metric='10'
    set network.wan.multipath='on'
