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
		init() {
			try {
				this.initDarkMode();
				this.initAdvancedToggle();
				this.initTooltips();
				this.initAccessibility();
				this.initAnimations();
				this.initFormValidation();
				this.initTableEnhancements();
				this.initPerformanceOptimizations();
			} catch (error) {
				console.error('OMRTheme initialization error:', error);
			}
		},

		// Dark mode support with modern syntax
		initDarkMode() {
			const toggleBtn = document.getElementById('theme-toggle');
			if (!toggleBtn) return;

			// Check saved preference using nullish coalescing
			const savedTheme = localStorage.getItem('omr-theme');
			const prefersDark = window.matchMedia('(prefers-color-scheme: dark)').matches;
			const isDark = savedTheme === 'dark' || (!savedTheme && prefersDark);

			if (isDark) {
				document.body.classList.add('dark-mode');
			}

			// Modern arrow function with optional chaining
			toggleBtn.addEventListener('click', () => {
				const isDarkMode = document.body.classList.toggle('dark-mode');
				localStorage.setItem('omr-theme', isDarkMode ? 'dark' : 'light');
			}, { passive: true });

			// Listen for system theme changes with modern syntax
			const darkModeQuery = window.matchMedia('(prefers-color-scheme: dark)');
			darkModeQuery.addEventListener('change', (e) => {
				if (!localStorage.getItem('omr-theme')) {
					document.body.classList.toggle('dark-mode', e.matches);
				}
			});
		},

		// Advanced settings toggle with modern syntax
		initAdvancedToggle() {
			const toggleBtn = document.getElementById('advanced-toggle');
			if (!toggleBtn) return;

			// Check saved preference
			const showAdvanced = localStorage.getItem('omr-show-advanced') === 'true';
			if (showAdvanced) {
				document.body.classList.add('show-advanced');
			}

			toggleBtn.addEventListener('click', () => {
				const isShown = document.body.classList.toggle('show-advanced');
				localStorage.setItem('omr-show-advanced', isShown);
			}, { passive: true });
		},

		// Tooltip support with modern syntax and optional chaining
		initTooltips() {
			const tooltipElements = document.querySelectorAll('[data-tooltip]');

			tooltipElements.forEach((el) => {
				el.addEventListener('mouseenter', () => {
					const tooltipText = el.getAttribute('data-tooltip');
					if (!tooltipText) return;

					const tooltip = document.createElement('div');
					tooltip.className = 'tooltip show';
					// Security: Use textContent to prevent XSS injection via data-tooltip attribute
				const tooltipInner = document.createElement('div');
				tooltipInner.className = 'tooltip-inner';
				tooltipInner.textContent = tooltipText;
				tooltip.appendChild(tooltipInner);
					document.body.appendChild(tooltip);

					const rect = el.getBoundingClientRect();
					tooltip.style.top = `${rect.top - tooltip.offsetHeight - 5}px`;
					tooltip.style.left = `${rect.left + (rect.width / 2) - (tooltip.offsetWidth / 2)}px`;

					el._tooltip = tooltip;
				}, { passive: true });

				el.addEventListener('mouseleave', () => {
					el._tooltip?.remove();
					el._tooltip = null;
				}, { passive: true });
			});
		},

		// Accessibility enhancements with modern syntax
		initAccessibility() {
			// Add keyboard navigation for dropdowns
			const dropdowns = document.querySelectorAll('.dropdown');

			dropdowns.forEach((dropdown) => {
				const toggle = dropdown.querySelector('.dropdown-toggle');
				const menu = dropdown.querySelector('.dropdown-menu');

				if (!toggle || !menu) return;

				toggle.addEventListener('click', (e) => {
					e.preventDefault();
					menu.classList.toggle('show');
				}, { passive: false });

				toggle.addEventListener('keydown', (e) => {
					if (e.key === 'Enter' || e.key === ' ') {
						e.preventDefault();
						menu.classList.toggle('show');
					}
				});

				// Close on escape with modern event delegation
				const escapeHandler = (e) => {
					if (e.key === 'Escape' && menu.classList.contains('show')) {
						menu.classList.remove('show');
						toggle.focus();
					}
				};
				document.addEventListener('keydown', escapeHandler);

				// Close when clicking outside
				const outsideClickHandler = (e) => {
					if (!dropdown.contains(e.target)) {
						menu.classList.remove('show');
					}
				};
				document.addEventListener('click', outsideClickHandler, { passive: true });
			});

			// Ensure all interactive elements are keyboard accessible
			const interactiveElements = document.querySelectorAll('a, button, input, select, textarea, [tabindex]');
			interactiveElements.forEach((el) => {
				const tagName = el.tagName;
				if (!el.hasAttribute('tabindex') &&
				    !['INPUT', 'SELECT', 'TEXTAREA'].includes(tagName)) {
					el.setAttribute('tabindex', '0');
				}
			});
		},

		// Smooth animations with GPU acceleration and modern syntax
		initAnimations() {
			// Add GPU acceleration to animated elements
			const animatedElements = document.querySelectorAll('.cbi-button, .main-menu a, .card');
			animatedElements.forEach((el) => el.classList.add('gpu-accelerated'));

			// Intersection observer for lazy animations with modern options
			if ('IntersectionObserver' in window) {
				const observer = new IntersectionObserver((entries) => {
					entries.forEach((entry) => {
						if (entry.isIntersecting) {
							entry.target.classList.add('animate-in');
							// Unobserve after animation to improve performance
							observer.unobserve(entry.target);
						}
					});
				}, {
					threshold: 0.1,
					rootMargin: '50px'
				});

				document.querySelectorAll('.cbi-section, .card').forEach((el) => {
					observer.observe(el);
				});
			}
		},

		// Form validation enhancements with modern syntax
		initFormValidation() {
			const forms = document.querySelectorAll('form');

			forms.forEach((form) => {
				// Real-time validation
				const inputs = form.querySelectorAll('input[required], select[required], textarea[required]');

				inputs.forEach((input) => {
					input.addEventListener('blur', () => {
						input.classList.toggle('is-invalid', !input.validity.valid);
					}, { passive: true });

					input.addEventListener('input', () => {
						if (input.classList.contains('is-invalid') && input.validity.valid) {
							input.classList.remove('is-invalid');
						}
					}, { passive: true });
				});

				// Form submit validation
				form.addEventListener('submit', (e) => {
					const invalidInputs = Array.from(inputs).filter(input => !input.validity.valid);

					if (invalidInputs.length > 0) {
						e.preventDefault();
						invalidInputs.forEach(input => input.classList.add('is-invalid'));
						// Focus first invalid field with smooth scroll
						invalidInputs[0]?.focus({ preventScroll: false });
						invalidInputs[0]?.scrollIntoView({ behavior: 'smooth', block: 'center' });
					}
				});
			});
		},

		// Table enhancements with modern syntax
		initTableEnhancements() {
			const tables = document.querySelectorAll('table.cbi-section-table');

			tables.forEach((table) => {
				// Make tables responsive
				if (!table.parentElement?.classList.contains('table-responsive')) {
					const wrapper = document.createElement('div');
					wrapper.className = 'table-responsive';
					table.parentNode.insertBefore(wrapper, table);
					wrapper.appendChild(table);
				}

				// Add sorting capability to headers
				const headers = table.querySelectorAll('th');
				headers.forEach((header, index) => {
					const headerText = header.textContent?.trim();
					if (headerText) {
						header.style.cursor = 'pointer';
						header.setAttribute('role', 'button');
						header.setAttribute('aria-label', `Sort by ${headerText}`);

						header.addEventListener('click', () => {
							this.sortTable(table, index);
						}, { passive: true });
					}
				});
			});
		},

		// Table sorting with modern syntax and better performance
		sortTable(table, columnIndex) {
			const tbody = table.querySelector('tbody');
			if (!tbody) return;

			const rows = Array.from(tbody.querySelectorAll('tr'));
			const isAscending = table.getAttribute('data-sort-order') !== 'asc';

			// Use modern sort with destructuring
			rows.sort((a, b) => {
				const aVal = a.cells[columnIndex]?.textContent?.trim() ?? '';
				const bVal = b.cells[columnIndex]?.textContent?.trim() ?? '';

				// Try numeric comparison first
				const aNum = parseFloat(aVal);
				const bNum = parseFloat(bVal);

				if (!isNaN(aNum) && !isNaN(bNum)) {
					return isAscending ? aNum - bNum : bNum - aNum;
				}

				// Fall back to string comparison with locale
				return isAscending ? aVal.localeCompare(bVal) : bVal.localeCompare(aVal);
			});

			// Use DocumentFragment for better performance
			const fragment = document.createDocumentFragment();
			rows.forEach((row) => fragment.appendChild(row));
			tbody.appendChild(fragment);

			// Update sort indicator
			table.setAttribute('data-sort-order', isAscending ? 'asc' : 'desc');

			// Visual feedback with modern syntax
			const headers = table.querySelectorAll('th');
			headers.forEach((h, i) => {
				h.classList.remove('sorted-asc', 'sorted-desc');
				if (i === columnIndex) {
					h.classList.add(isAscending ? 'sorted-asc' : 'sorted-desc');
				}
			});
		},

		// Modern performance optimizations
		initPerformanceOptimizations() {
			// Lazy load images with modern loading attribute
			const images = document.querySelectorAll('img:not([loading])');
			images.forEach((img) => {
				img.setAttribute('loading', 'lazy');
				img.setAttribute('decoding', 'async');
			});

			// Use passive event listeners for scroll performance
			const scrollElements = document.querySelectorAll('[onscroll]');
			scrollElements.forEach((el) => {
				const scrollHandler = el.getAttribute('onscroll');
				if (scrollHandler) {
					el.removeAttribute('onscroll');
					// Security: Disabled new Function() to prevent code injection
				// Inline onscroll handlers should be migrated to addEventListener
				console.warn('Inline onscroll attribute detected but not executed for security reasons. Use addEventListener instead.');
				}
			});
		},

		// Utility: Modern debounce function with arrow syntax
		debounce(func, wait) {
			let timeout;
			return (...args) => {
				clearTimeout(timeout);
				timeout = setTimeout(() => func.apply(this, args), wait);
			};
		},

		// Show notification with modern syntax
		showNotification(message, type = 'info') {
			const notification = document.createElement('div');
			notification.className = `alert alert-${type}`;
			notification.textContent = message;
			Object.assign(notification.style, {
				position: 'fixed',
				top: '20px',
				right: '20px',
				zIndex: '9999',
				minWidth: '300px',
				animation: 'slideInRight 0.3s ease'
			});

			document.body.appendChild(notification);

			setTimeout(() => {
				notification.style.animation = 'slideOutRight 0.3s ease';
				setTimeout(() => notification.remove(), 300);
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
