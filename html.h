#pragma once

const char *uploadHtml = R"literal(
  <!DOCTYPE html>
  <head><link rel="stylesheet" href="style.css"></head>
  <body>
    <div id='bodydiv'>
    <div><img src='logo.jpg' width='300' height='73' onerror="this.style.display='none'"></div>
    <h1>Firmware Update</h1>
    <a href=/>Home</a>
    <hr>
    <form method='POST' enctype='multipart/form-data' id='upload-form'>
    <div class="container-box">
    <div class='labelup'>
    <label for='fileup'>Choose firmware...</label>
    <input name='file' type='file' id='fileup' class='hide-item' name='update'>
    </div>
    <div class='box' id='fname'></div>
    <div>
    <div class='labelup' onclick="submit_update();">Update</div>
    <input type='submit' class='hide-item' value='Update'>
    </div>
    </div>
    </form>
    <hr>
    <br>
    <div id='prg' style='width:0;color:white;text-align:center'>0%</div>
    </div>
  </body>
  <script>
    var prg = document.getElementById('prg');
    var form = document.getElementById('upload-form');
    form.addEventListener('submit', el=>{
      el.preventDefault();
      submit_update();
    }
    function submit_update() {
      prg.style.backgroundColor = 'blue';
      var data = new FormData(form);
      var req = new XMLHttpRequest();
      var fsize = document.getElementById('file').files[0].size;
      req.open('POST', '/update?size=' + fsize);
      req.upload.addEventListener('progress', p=>{
        let w = Math.round(p.loaded/p.total*100) + '%';
          if(p.lengthComputable){
             prg.innerHTML = w;
             prg.style.width = w;
          }
          if(w == '100%') prg.style.backgroundColor = 'black';
      });
      req.send(data);
     });
  </script>
</body>
</html>
)literal";


const char *fileHtml = R"literal(
<html>
<head>
 <link rel="stylesheet" href="style.css">
 <style>
  .fdel { cursor:pointer; color:red; }
 </style>
</head>
<body>
<div id='bodydiv'>
<div><img src='logo.jpg' width='300' height='73' onerror="this.style.display='none'"></div>
<h1>File Upload</h1>
<a href=/>Home</a>&nbsp;&nbsp;<a href=# onclick='formatar();'>Format flash</a>
<hr>
<div id='entrypoint'>
<form id='upform' method='POST' enctype='multipart/form-data'>
<div class='container-box'>
<div class='labelup'>
<label for='fileup'>Choose file...</label>
<input name='file' type='file' id='fileup' class='hide-item' multiple>
</div>
<div class='box' id='fname'></div>
<div class='labelup' onclick="submit_form();">Upload</div>
<input type='submit' class='hide-item' value='Upload'>
</div>
</form>
</div>
<div id='percent'></div>
<hr>
<script>
const button = document.getElementById('fileup');
const fname = document.getElementById('fname');
button.addEventListener('change', () => {
  for (const file of button.files) {
    fname.innerHTML = file.name + ' (' + file.size + ')';
  }
});
const form = document.querySelector('form');
form.addEventListener('submit', handleSubmit);
function handleSubmit(event) {
    event.preventDefault();
    submit_form();
}
function submit_form() {
    const url = '/fs-upload';
    const method = 'post';
    const xhr = new XMLHttpRequest();
    const data = new FormData(form);
    xhr.upload.addEventListener('progress', (event) => {
      document.getElementById('percent').innerHTML = 'Progress: ' + ((event.loaded / event.total) * 100).toFixed(2,) + '%';
      if(event.loaded == event.total) {
        setTimeout(function() {
          location.reload();
        }, 1000);
      }
    });
    xhr.open(method, url);
    xhr.send(data);
    document.getElementById('entrypoint').style.display = "none";
}
function fdel(fname) {
  console.log('Delete: ' + fname);
  const url = '/delete?arq=' + fname;
  const xhr = new XMLHttpRequest();
  xhr.open('GET', url);
  xhr.send();
  setTimeout(function() {
    location.reload();
  }, 1500);
}
function formatar() {
  if(confirm('This will be wipe all configurations and files on the storage, confirm?')) {
    window.location.href('/format');
  }
}
</script>
)literal";

// <EOF>