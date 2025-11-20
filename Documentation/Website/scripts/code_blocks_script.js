// Initialize syntax highlighting
hljs.highlightAll();

// Create and show copy notification
function showCopyNotification() {
    // Check if notification already exists
    let notification = document.querySelector('.copy-notification');

    if (!notification) {
        // Create notification element
        notification = document.createElement('div');
        notification.className = 'copy-notification';
        notification.textContent = 'Copied to clipboard!';
        document.body.appendChild(notification);
    }

    // Show notification
    setTimeout(() => {
        notification.classList.add('show');
    }, 10);

    // Hide notification after 2 seconds
    setTimeout(() => {
        notification.classList.remove('show');
    }, 2000);
}

// Add copy buttons to all code blocks
function addCopyButtons() {
    document.querySelectorAll('pre').forEach((preElement) => {
        // Check if button already exists
        if (preElement.querySelector('.copy-btn')) {
            return;
        }

        // Wrap pre in a container if not already wrapped
        if (!preElement.parentElement.classList.contains('code-container')) {
            const container = document.createElement('div');
            container.className = 'code-container';
            preElement.parentNode.insertBefore(container, preElement);
            container.appendChild(preElement);
        }

        // Create copy button
        const button = document.createElement('button');
        button.className = 'copy-btn';
        button.setAttribute('aria-label', 'Copy code');

        // Create image element for copy icon
        const copyIcon = document.createElement('img');
        copyIcon.src = '/Documentation/Website/media/copy_icon.png';
        copyIcon.alt = 'Copy';
        button.appendChild(copyIcon);

        button.addEventListener('click', () => {
            const code = preElement.querySelector('code');
            const text = code.textContent;

            navigator.clipboard.writeText(text).then(() => {
                // Replace with tick icon
                copyIcon.src = '/Documentation/Website/media/tick.png';
                copyIcon.alt = 'Copied';
                button.classList.add('copied');

                // Show notification popup
                showCopyNotification();

                setTimeout(() => {
                    // Restore copy icon
                    copyIcon.src = '/Documentation/Website/media/copy_icon.png';
                    copyIcon.alt = 'Copy';
                    button.classList.remove('copied');
                }, 2000);
            }).catch(err => {
                console.error('Failed to copy:', err);
                // Optionally show an error icon
                setTimeout(() => {
                    copyIcon.src = '/Documentation/Website/media/copy_icon.png';
                    copyIcon.alt = 'Copy';
                }, 2000);
            });
        });

        preElement.parentElement.appendChild(button);
    });
}

// Initialize copy buttons on page load
addCopyButtons();