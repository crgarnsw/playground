var http = require('http'),
    fs = require('fs'),
    url = require('url');

http.createServer(function(request, response){
  var path = url.parse(request.url).pathname;

  if(path.indexOf( "init" ) > -1) {
    response.writeHead( 200, { "Content-Type": "text/plain" });
    response.end( '[{ "name": "Nicks Tree",   "idx":"1", "val": "true"},' +
                  ' { "name": "Main Tree",    "idx":"2", "val": "false"},' +
                  ' { "name": "Maddies Tree", "idx":"3", "val": "true"},' +
                  ' { "name": "Dining Tree",  "idx":"4", "val": "false"}]' );
  } else if(path.indexOf( "switch" ) > -1) {
    response.writeHead(200, {"Content-Type": "text/plain"});
    response.end('OK');
  } else {
    path = path === '/' ? 'index.htm' : path.substring(1,path.length);

console.log("path: "  + path);

    fs.readFile(path, function(err, file) {  
      if(err) {  
        // write an error response or nothing here  
        console.error("Error reading file: " + path);
        return;  
      }  
      response.writeHead(200, { 'Content-Type': 'text/html' });  
      response.end(file, "utf-8");  
    });
  }
}).listen(8001);
