document.addEventListener('DOMContentLoaded', function () {
    const petForm = document.getElementById('petForm');

    petForm.addEventListener('submit', function (event) {
        event.preventDefault(); // Prevent default form submission behavior

        const formData = new FormData(petForm);

        // Send pet information to Flask backend using AJAX
        fetch('/submit_pet_info', {
            method: 'POST',
            body: formData
        })
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok');
            }
            return response.json(); // Parse JSON response
        })
        .then(data => {
            // Handle response from Flask
            // Render charts using received data
            renderCharts(data);
        })
        .catch(error => {
            console.error('Error:', error);
        });
    });

    function renderCharts(data) {
        // Use Chart.js to render charts based on the processed data
        // Example:
        const ctx = document.getElementById('chartContainer').getContext('2d');
        const myChart = new Chart(ctx, {
            type: 'bar',
            data: {
                labels: ['Pet 1', 'Pet 2', 'Pet 3'],
                datasets: [{
                    label: 'Age',
                    data: [data.pet1.age, data.pet2.age, data.pet3.age],
                    backgroundColor: [
                        'rgba(255, 99, 132, 0.2)',
                        'rgba(54, 162, 235, 0.2)',
                        'rgba(255, 206, 86, 0.2)'
                    ],
                    borderColor: [
                        'rgba(255, 99, 132, 1)',
                        'rgba(54, 162, 235, 1)',
                        'rgba(255, 206, 86, 1)'
                    ],
                    borderWidth: 1
                }]
            },
            options: {
                scales: {
                    y: {
                        beginAtZero: true
                    }
                }
            }
        });
    }
});
