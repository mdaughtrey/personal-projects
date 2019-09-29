var http = require("http");
var url = require("url");

function start(route, handle) {
    function onRequest(request, response) {
        var postData = '';
        var pathname = url.parse(request.url).pathname;
        console.log("Request for " + pathname + " received.");
        request.setEncoding('utf8');

        request.addListener("data", function(postDataChunk) {
            postData += postDataChunk;
            console.log("got chunk");
        });

        request.addListener("end", function() {
            route(handle, pathname, response, postData);
        });


//        response.writeHead(200, {"Content-Type": "text/plain"});
//       response.write(route(handle, pathname));
//        response.end();
    }

    http.createServer(onRequest).listen(41000);
    console.log("Server started.");
}

exports.start = start;
