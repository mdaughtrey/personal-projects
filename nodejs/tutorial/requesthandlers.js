//var exec = require("child_process").exec;

function start(response) {
    console.log("Handler start");
    var body = '<html><head><meta http-equiv="Content-Type" content="text/html;charset=UTF-8" /></head>' +
        '<body><form action="/upload" method="post">' +
        '<textarea name="text" rows="20" cols="60"></textarea>'+
        '<input type="submit" value="Submit text" />'+
        '</form></body></html>';
     response.writeHead(200, {"Content-Type": "text/html"});
     response.write(body);
     response.end();


//    exec("ls -lah", function(error, stdout, stderr) {
//        response.writeHead(200, {"Content-Type": "text/plain"});
//        response.write(stdout);
//        response.end();
//    });
//    return "Started";
}

function upload(response, postdata) {
    console.log("Handler upload");
    response.writeHead(200, {"Content-Type": "text/plain"});
    response.write("You sent " + postdata);
    response.end();
}

exports.start = start;
exports.upload = upload;

