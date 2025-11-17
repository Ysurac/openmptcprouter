/**
 * OpenMPTCProuter Optimized Theme - JavaScript Enhancements
 * Copyright 2025 OpenMPTCProuter Optimized
 * Licensed under GPL-3.0
 */

(function() {
	'use strict';

	// Theme controller
	const OMRTheme = {
		// Initialize theme
		init: function() {
			this.initDarkMode();
			this.initAdvancedToggle();
			this.initTooltips();
			this.initAccessibility();
			this.initAnimations();
			this.initFormValidation();
			this.initTableEnhancements();
		},

		// Dark mode support
		initDarkMode: function() {
			const toggleBtn = document.getElementById('theme-toggle');
			if (!toggleBtn) return;

			// Check saved preference
			const savedTheme = localStorage.getItem('omr-theme');
			const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
			const isDark = savedTheme === 'dark' || (!savedTheme && prefersDark);

			if (isDark) {
				document.body.classList.add('dark-mode');
			}

			toggleBtn.addEventListener('click', function() {
				const isDarkMode = document.body.classList.toggle('dark-mode');
				localStorage.setItem('omr-theme', isDarkMode ? 'dark' : 'light');
			});

			// Listen for system theme changes
			window.matchMedia('(prefers-color-scheme: dark)').addEventListener('change', function(e) {
				if (!localStorage.getItem('omr-theme')) {
					document.body.classList.toggle('dark-mode', e.matches);
				}
			});
		},

		// Advanced settings toggle
		initAdvancedToggle: function() {
			const toggleBtn = document.getElementById('advanced-toggle');
			if (!toggleBtn) return;

			// Check saved preference
			const showAdvanced = localStorage.getItem('omr-show-advanced') === 'true';
			if (showAdvanced) {
				document.body.classList.add('show-advanced');
			}

			toggleBtn.addEventListener('click', function() {
				const isShown = document.body.classList.toggle('show-advanced');
				localStorage.setItem('omr-show-advanced', isShown);
			});
		},

		// Tooltip support
		initTooltips: function() {
			const tooltipElements = document.querySelectorAll('[data-tooltip]');
			
			tooltipElements.forEach(function(el) {
				el.addEventListener('mouseenter', function() {
					const tooltipText = el.getAttribute('data-tooltip');
					if (!tooltipText) return;

					const tooltip = document.createElement('div');
					tooltip.className = 'tooltip show';
					tooltip.innerHTML = '<div class="tooltip-inner">' + tooltipText + '</div>';
					document.body.appendChild(tooltip);

					const rect = el.getBoundingClientRect();
					tooltip.style.top = (rect.top - tooltip.offsetHeight - 5) + 'px';
					tooltip.style.left = (rect.left + (rect.width / 2) - (tooltip.offsetWidth / 2)) + 'px';

					el._tooltip = tooltip;
				});

				el.addEventListener('mouseleave', function() {
					if (el._tooltip) {
						el._tooltip.remove();
						el._tooltip = null;
					}
				});
			});
		},

		// Accessibility enhancements
		initAccessibility: function() {
			// Add keyboard navigation for dropdowns
			const dropdowns = document.querySelectorAll('.dropdown');
			
			dropdowns.forEach(function(dropdown) {
				const toggle = dropdown.querySelector('.dropdown-toggle');
				const menu = dropdown.querySelector('.dropdown-menu');
				
				if (!toggle || !menu) return;

				toggle.addEventListener('click', function(e) {
					e.preventDefault();
					menu.classList.toggle('show');
				});

				toggle.addEventListener('keydown', function(e) {
					if (e.key === 'Enter' || e.key === ' ') {
						e.preventDefault();
						menu.classList.toggle('show');
					}
				});

				// Close on escape
				document.addEventListener('keydown', function(e) {
					if (e.key === 'Escape' && menu.classList.contains('show')) {
						menu.classList.remove('show');
						toggle.focus();
					}
				});

				// Close when clicking outside
				document.addEventListener('click', function(e) {
					if (!dropdown.contains(e.target)) {
						menu.classList.remove('show');
					}
				});
			});

			// Ensure all interactive elements are keyboard accessible
			const interactiveElements = document.querySelectorAll('a, button, input, select, textarea, [tabindex]');
			interactiveElements.forEach(function(el) {
				if (!el.hasAttribute('tabindex') && el.tagName !== 'INPUT' && el.tagName !== 'SELECT' && el.tagName !== 'TEXTAREA') {
					el.setAttribute('tabindex', '0');
				}
			});
		},

		// Smooth animations with GPU acceleration
		initAnimations: function() {
			// Add GPU acceleration to animated elements
			const animatedElements = document.querySelectorAll('.cbi-button, .main-menu a, .card');
			animatedElements.forEach(function(el) {
				el.classList.add('gpu-accelerated');
			});

			// Intersection observer for lazy animations
			if ('IntersectionObserver' in window) {
				const observer = new IntersectionObserver(function(entries) {
					entries.forEach(function(entry) {
						if (entry.isIntersecting) {
							entry.target.classList.add('animate-in');
						}
					});
				}, {
					threshold: 0.1
				});

				document.querySelectorAll('.cbi-section, .card').forEach(function(el) {
					observer.observe(el);
				});
			}
		},

		// Form validation enhancements
		initFormValidation: function() {
			const forms = document.querySelectorAll('form');
			
			forms.forEach(function(form) {
				// Real-time validation
				const inputs = form.querySelectorAll('input[required], select[required], textarea[required]');
				
				inputs.forEach(function(input) {
					input.addEventListener('blur', function() {
						if (!input.validity.valid) {
							input.classList.add('is-invalid');
						} else {
							input.classList.remove('is-invalid');
						}
					});

					input.addEventListener('input', function() {
						if (input.classList.contains('is-invalid') && input.validity.valid) {
							input.classList.remove('is-invalid');
						}
					});
				});

				// Form submit validation
				form.addEventListener('submit', function(e) {
					let isValid = true;
					
					inputs.forEach(function(input) {
						if (!input.validity.valid) {
							input.classList.add('is-invalid');
							isValid = false;
						}
					});

					if (!isValid) {
						e.preventDefault();
						// Focus first invalid field
						const firstInvalid = form.querySelector('.is-invalid');
						if (firstInvalid) {
							firstInvalid.focus();
						}
					}
				});
			});
		},

		// Table enhancements
		initTableEnhancements: function() {
			const tables = document.querySelectorAll('table.cbi-section-table');
			
			tables.forEach(function(table) {
				// Make tables responsive
				if (!table.parentElement.classList.contains('table-responsive')) {
					const wrapper = document.createElement('div');
					wrapper.className = 'table-responsive';
					table.parentNode.insertBefore(wrapper, table);
					wrapper.appendChild(table);
				}

				// Add sorting capability to headers
				const headers = table.querySelectorAll('th');
				headers.forEach(function(header, index) {
					if (header.textContent.trim()) {
						header.style.cursor = 'pointer';
						header.setAttribute('role', 'button');
						header.setAttribute('aria-label', 'Sort by ' + header.textContent);
						
						header.addEventListener('click', function() {
							OMRTheme.sortTable(table, index);
						});
					}
				});
			});
		},

		// Table sorting
		sortTable: function(table, columnIndex) {
			const tbody = table.querySelector('tbody');
			if (!tbody) return;

			const rows = Array.from(tbody.querySelectorAll('tr'));
			const isAscending = table.getAttribute('data-sort-order') !== 'asc';
			
			rows.sort(function(a, b) {
				const aVal = a.cells[columnIndex]?.textContent.trim() || '';
				const bVal = b.cells[columnIndex]?.textContent.trim() || '';
				
				// Try numeric comparison first
				const aNum = parseFloat(aVal);
				const bNum = parseFloat(bVal);
				
				if (!isNaN(aNum) && !isNaN(bNum)) {
					return isAscending ? aNum - bNum : bNum - aNum;
				}
				
				// Fall back to string comparison
				return isAscending ? aVal.localeCompare(bVal) : bVal.localeCompare(aVal);
			});

			// Update table
			rows.forEach(function(row) {
				tbody.appendChild(row);
			});

			// Update sort indicator
			table.setAttribute('data-sort-order', isAscending ? 'asc' : 'desc');
			
			// Visual feedback
			const headers = table.querySelectorAll('th');
			headers.forEach(function(h, i) {
				h.classList.remove('sorted-asc', 'sorted-desc');
				if (i === columnIndex) {
					h.classList.add(isAscending ? 'sorted-asc' : 'sorted-desc');
				}
			});
		},

		// Utility: Debounce function
		debounce: function(func, wait) {
			let timeout;
			return function executedFunction() {
				const context = this;
				const args = arguments;
				clearTimeout(timeout);
				timeout = setTimeout(function() {
					func.apply(context, args);
				}, wait);
			};
		},

		// Show notification
		showNotification: function(message, type) {
			type = type || 'info';
			const notification = document.createElement('div');
			notification.className = 'alert alert-' + type;
			notification.textContent = message;
			notification.style.position = 'fixed';
			notification.style.top = '20px';
			notification.style.right = '20px';
			notification.style.zIndex = '9999';
			notification.style.minWidth = '300px';
			notification.style.animation = 'slideInRight 0.3s ease';

			document.body.appendChild(notification);

			setTimeout(function() {
				notification.style.animation = 'slideOutRight 0.3s ease';
				setTimeout(function() {
					notification.remove();
				}, 300);
			}, 3000);
		}
	};

	// Export to global scope
	window.OMRTheme = OMRTheme;

	// Auto-init on DOMContentLoaded
	if (document.readyState === 'loading') {
		document.addEventListener('DOMContentLoaded', function() {
			OMRTheme.init();
		});
	} else {
		OMRTheme.init();
	}

	// Add animation keyframes
	const style = document.createElement('style');
	style.textContent = `
		@keyframes slideInRight {
			from {
				transform: translateX(100%);
				opacity: 0;
			}
			to {
				transform: translateX(0);
				opacity: 1;
			}
		}
		
		@keyframes slideOutRight {
			from {
				transform: translateX(0);
				opacity: 1;
			}
			to {
				transform: translateX(100%);
				opacity: 0;
			}
		}
		
		.animate-in {
			animation: fadeIn 0.5s ease;
		}
		
		@keyframes fadeIn {
			from {
				opacity: 0;
				transform: translateY(20px);
			}
			to {
				opacity: 1;
				transform: translateY(0);
			}
		}
		
		.is-invalid {
			border-color: var(--danger-color) !important;
			box-shadow: 0 0 0 3px rgba(220, 53, 69, 0.1) !important;
		}
		
		.table-responsive {
			overflow-x: auto;
			-webkit-overflow-scrolling: touch;
		}
		
		th.sorted-asc::after {
			content: " ▲";
			font-size: 0.8em;
			color: var(--primary-color);
		}
		
		th.sorted-desc::after {
			content: " ▼";
			font-size: 0.8em;
			color: var(--primary-color);
		}
	`;
	document.head.appendChild(style);

})();
