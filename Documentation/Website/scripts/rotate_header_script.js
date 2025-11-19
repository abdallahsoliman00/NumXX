const header = document.querySelector('header');
let angle = 135;

function animateGradient() {
    angle = (angle + 0.1) % 360;
    header.style.background = `linear-gradient(${angle}deg, rgba(122, 45, 147, 0.6) 0%, rgba(0, 68, 128, 0.6) 100%)`;
    requestAnimationFrame(animateGradient);
}

animateGradient();