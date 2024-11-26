document.addEventListener('DOMContentLoaded', () => {
  const takePhotoButton = document.getElementById('takePhoto');
  const cameraInput = document.getElementById('cameraInput');
  const imagePreview = document.getElementById('imagePreview');
  const output = document.getElementById('output');
  const addAnnotationButton = document.getElementById('addAnnotation');
  const downloadLink = document.getElementById('downloadLink');
  const recognition = new (window.SpeechRecognition || window.webkitSpeechRecognition)();
  recognition.lang = 'en-US';
  recognition.continuous = false;
  recognition.interimResults = false;
  takePhotoButton.addEventListener('click', () => {
    cameraInput.click();
  });
  cameraInput.addEventListener('change', (event) => {
    const file = event.target.files[0];
    if (file) {
      const reader = new FileReader();
      reader.onload = function(e) {
        imagePreview.src = e.target.result;
        imagePreview.hidden = false;
        // Start voice recognition to name the picture
        output.textContent = 'Listening for image name...';
        recognition.start();
      };
      reader.readAsDataURL(file);
    }
  });
  recognition.onresult = (event) => {
    const name = event.results[0][0].transcript;
    output.textContent = `Image named: ${name}`;
    alert(`You named the image: ${name}`);
    recognition.stop(); 
    addAnnotationButton.style.display = 'inline';
    addAnnotationButton.onclick = () => startAnnotation(name);
  };
  recognition.onerror = (event) => {
    output.textContent = `Error occurred in recognition: ${event.error}`;
  };
  recognition.onspeechend = () => {
    recognition.stop();
  };
  function startAnnotation(imageName) {
    output.textContent = 'Listening for annotation...';
    recognition.start();
    recognition.onresult = (event) => {
      const annotation = event.results[0][0].transcript;
      output.textContent = `Annotation added: ${annotation}`;
      recognition.stop();
      saveAnnotation(imageName, annotation);
    };
  }
  function saveAnnotation(imageName, annotation) {
    const blob = new Blob([`Image: ${imageName}\nAnnotation: ${annotation}`], { type: 'text/plain' });
    const url = URL.createObjectURL(blob);
    downloadLink.href = url;
    downloadLink.download = `${imageName}_annotation.txt`;
    downloadLink.textContent = 'Download your annotation file';
    downloadLink.style.display = 'block';
  }
});
