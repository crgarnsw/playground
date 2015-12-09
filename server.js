var http = require('http'),
      fs = require('fs'),
     url = require('url'),
 choices = ["hello world", "goodbye world"];

http.createServer(function(request, response){
    var path = url.parse(request.url).pathname;

console.log('path: ' + path);
    if(path.indexOf( "switch" ) > -1){
        console.log("request recieved");
        var string = choices[Math.floor(Math.random()*choices.length)];
        console.log("string '" + string + "' chosen");
        response.writeHead(200, {"Content-Type": "text/plain"});
        response.end(string);
        console.log("string sent");
    }else{
        path = path === '/' ? 'index.htm' : path.substring(1,path.length);
console.log('path2: ' + path);

        fs.readFile(path, function(err, file) {  
            if(err) {  
                // write an error response or nothing here  
                return;  
            }  
            response.writeHead(200, { 'Content-Type': 'text/html' });  
            response.end(file, "utf-8");  
        });
    }
}).listen(8001);
console.log("server initialized");
