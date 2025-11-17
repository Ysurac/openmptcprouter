# LuCI Theme - OpenMPTCProuter Optimized

A modern, clean, and accessible theme for OpenMPTCProuter's LuCI web interface.

## Features

### 🎨 Modern Design
- Clean, professional interface with gradient accents
- Smooth animations and transitions
- Card-based layout for better content organization
- Responsive design that works on all devices

### 🌓 Dark Mode Support
- Automatic dark mode based on system preferences
- Manual toggle for user preference
- Smooth transitions between modes
- Optimized colors for both light and dark themes

### ♿ Accessibility
- WCAG 2.1 compliant
- Proper ARIA labels and roles
- Keyboard navigation support
- Focus visible indicators
- High contrast mode support
- Screen reader friendly

### 📱 Responsive Design
- Mobile-first approach
- Optimized for tablets and phones
- Touch-friendly interface
- Adaptive navigation

### ⚡ Performance
- GPU-accelerated animations
- Optimized CSS with CSS variables
- Minimal JavaScript footprint
- Lazy-loaded animations
- Print-optimized styles

### 🎯 Advanced Features
- Toggle for advanced settings (hide/show complex options)
- Sortable tables with click-to-sort headers
- Real-time form validation
- Tooltips for better guidance
- Dropdown menus with keyboard support
- Loading states and animations

## File Structure

```
luci-theme-omr-optimized/
├── htdocs/
│   └── luci-static/
│       └── omr-optimized/
│           ├── cascade.css          # Main theme styles
│           ├── css/
│           │   ├── components.css   # Reusable components
│           │   └── utilities.css    # Utility classes
│           ├── js/
│           │   └── theme.js         # Theme functionality
│           └── images/
│               └── favicon.ico
├── luasrc/
│   └── view/
│       └── themes/
│           └── omr-optimized/
│               ├── header.htm       # Page header template
│               └── footer.htm       # Page footer template
├── uci-defaults/
│   └── 30_luci-theme-omr-optimized # Auto-activation script
└── Makefile
```

## CSS Variables

The theme uses CSS custom properties for easy customization:

```css
/* Colors */
--primary-color: #667eea
--success-color: #28a745
--warning-color: #ffc107
--danger-color: #dc3545

/* Spacing */
--spacing-xs: 4px
--spacing-sm: 8px
--spacing-md: 16px
--spacing-lg: 24px

/* Border radius */
--radius: 8px
--radius-lg: 12px

/* Shadows */
--shadow: 0 2px 8px rgba(0, 0, 0, 0.1)
--shadow-lg: 0 4px 16px rgba(0, 0, 0, 0.15)
```

## Utility Classes

The theme includes a comprehensive set of utility classes:

### Spacing
- `mt-{0-4}`, `mb-{0-4}`, `ml-{0-3}`, `mr-{0-3}` - Margins
- `p-{0-4}`, `px-{1-3}`, `py-{1-3}` - Padding

### Display
- `d-none`, `d-block`, `d-flex`, `d-grid` - Display types

### Flexbox
- `justify-{start|center|end|between}` - Justify content
- `align-{start|center|end}` - Align items
- `gap-{1-3}` - Gap between items

### Text
- `text-{left|center|right}` - Text alignment
- `text-{small|normal|large}` - Font sizes
- `font-{light|normal|medium|bold}` - Font weights
- `text-{muted|primary|success|warning|danger}` - Text colors

### Background
- `bg-{white|color|primary|success|warning|danger}` - Background colors

## Components

### Alerts
```html
<div class="alert alert-success">Success message</div>
<div class="alert alert-warning">Warning message</div>
<div class="alert alert-danger">Error message</div>
<div class="alert alert-info">Info message</div>
```

### Cards
```html
<div class="card">
  <div class="card-header">Title</div>
  <div class="card-body">Content</div>
  <div class="card-footer">Footer</div>
</div>
```

### Badges
```html
<span class="badge badge-primary">Primary</span>
<span class="badge badge-success">Success</span>
```

### Progress Bars
```html
<div class="progress">
  <div class="progress-bar" style="width: 75%"></div>
</div>
```

## JavaScript API

The theme includes a JavaScript API for enhanced functionality:

```javascript
// Show notification
OMRTheme.showNotification('Message', 'success');

// Sort table
OMRTheme.sortTable(tableElement, columnIndex);

// Initialize (called automatically)
OMRTheme.init();
```

## Browser Support

- Chrome/Edge: Latest 2 versions
- Firefox: Latest 2 versions
- Safari: Latest 2 versions
- Mobile browsers: iOS Safari, Chrome Mobile

## Accessibility Features

- Semantic HTML5 elements
- ARIA labels and roles
- Keyboard navigation
- Focus visible indicators
- Skip to content links
- High contrast mode support
- Screen reader announcements

## Performance Optimizations

- CSS variables for theme switching
- GPU-accelerated animations
- Debounced event handlers
- Intersection Observer for lazy animations
- Optimized paint and layout
- Reduced motion support

## Development

To modify the theme:

1. Edit CSS files in `htdocs/luci-static/omr-optimized/`
2. Edit templates in `luasrc/view/themes/omr-optimized/`
3. Edit JavaScript in `htdocs/luci-static/omr-optimized/js/`
4. Rebuild the package with OpenWrt build system

## License

GPL-3.0 - See LICENSE file for details

## Credits

Based on LuCI with enhancements for OpenMPTCProuter Optimized.
