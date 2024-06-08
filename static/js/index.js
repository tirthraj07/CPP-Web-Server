console.log("Hello World")
console.log("As you can see, Java Script also works!")

function capitalizeFirstLetter(string) {
    return string.charAt(0).toUpperCase() + string.slice(1);
}

fetch('/api/social-media')
.then(response => response.json())
.then(data => {
    const tbody = document.getElementById('tbody');
    for (const [key, value] of Object.entries(data)) {
        let innerHtml = `<tr><td><a target="_blank" href="${value}"> ${capitalizeFirstLetter(key)   }</a></td></tr>`;
        tbody.innerHTML += innerHtml;
    }
})
.catch(err => {
    console.error("Error : ", err);
})

const form = document.getElementById('myform');
form.addEventListener('submit',async (e)=>{
    e.preventDefault();
    
    const jsonPayload = {
        name: form['name'].value,
        email: form['email'].value
    }

    fetch('/api/form', {
        method: 'POST',
        headers:{
            "Content-Type":"application/json"
        },
        body: JSON.stringify(jsonPayload)
    })
    .then(response => response.json())
    .then(data => {
        alert(`Status: ${data["status"]}`);
    })  
    .catch(error => {
        alert("Error while fetching", error);
    })


})