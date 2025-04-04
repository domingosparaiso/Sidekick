#ifndef HTML_H
  #define HTML_H 1

const char *uploadHtml = R"literal(
  <!DOCTYPE html>
  <body style='width:480px'>
    <h1>Firmware Update</h1>
    <a href=/>Home</a>
    <hr>
    <form method='POST' enctype='multipart/form-data' id='upload-form'>
      <input type='file' id='file' name='update'>
      <input type='submit' value='Update'>
    </form>
    <hr>
    <br>
    <div id='prg' style='width:0;color:white;text-align:center'>0%</div>
  </body>
  <script>
    var prg = document.getElementById('prg');
    var form = document.getElementById('upload-form');
    form.addEventListener('submit', el=>{
      prg.style.backgroundColor = 'blue';
      el.preventDefault();
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
 <style>
  .fdel { cursor:pointer; color:red; }
 </style>
</head>
<body>
<h1>File Upload</h1>
<a href=/>Home</a>&nbsp;&nbsp;<a href=# onclick='formatar();'>Format flash</a>
<hr>
<div id='entrypoint'>
<form method='post' enctype='multipart/form-data'>
<input name='file' type='file' multiple>
<button type='submit'>Upload</button>
</form>
</div>
<div id='percent'></div>
<hr>
<script>
const form = document.querySelector('form');
form.addEventListener('submit', handleSubmit);
function handleSubmit(event) {
    event.preventDefault();
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

#endif // <EOF>