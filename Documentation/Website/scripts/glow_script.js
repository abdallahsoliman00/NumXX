// Enhanced glow effect with spillover for hero and doc-cards
document.addEventListener('DOMContentLoaded', function() {
    const glowElements = document.querySelectorAll('.hero, .doc-card, .section-title');

    // Track mouse position globally
    let globalMouseX = 0;
    let globalMouseY = 0;

    document.addEventListener('mousemove', function(event) {
        globalMouseX = event.clientX;
        globalMouseY = event.clientY;
    });

    // Apply glow effect to all elements
    function updateGlows() {
        glowElements.forEach(element => {
            const rect = element.getBoundingClientRect();

            // Calculate position relative to this element
            const x = globalMouseX - rect.left;
            const y = globalMouseY - rect.top;

            // Calculate distance from mouse to element center
            const centerX = rect.width / 2;
            const centerY = rect.height / 2;
            const distanceFromCenter = Math.sqrt(
                Math.pow(x - centerX, 2) + Math.pow(y - centerY, 2)
            );

            // Define glow radius (affects nearby elements)
            const glowRadius = 400;

            // Check if mouse is over this element or nearby
            const isOver = x >= 0 && x <= rect.width && y >= 0 && y <= rect.height;
            const isNearby = distanceFromCenter < glowRadius;

            if (isOver || isNearby) {
              const dx = x - centerX;
              const dy = y - centerY;
              const distance = Math.sqrt(dx*dx + dy*dy);
              const opacity = Math.max(0, 1 - (distance / glowRadius));

              if (element.classList.contains('section-title')) {
                // TEXT GLOW version - reduced intensity
                element.style.textShadow = `
                  ${dx * 0.05}px ${dy * 0.05}px 25px rgba(59, 248, 251, ${0.5 * opacity}),
                  0 0 35px rgba(59, 248, 251, ${0.4 * opacity})
                `;
              } else {
                // ORIGINAL BACKGROUND GLOW for cards & hero
                element.style.background = `radial-gradient(800px circle at ${x}px ${y}px, rgba(59, 248, 251, ${0.15 * opacity}), transparent 40%)`;
              }
            } else {
              if (element.classList.contains('section-title')) {
                element.style.textShadow = 'none';
              } else {
                element.style.background = 'none';
              }
            }

        });

        requestAnimationFrame(updateGlows);
    }

    updateGlows();
});