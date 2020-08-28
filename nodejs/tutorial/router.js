function route(handle, pathname, response, postdata) {
    console.log("routing " + pathname);
    console.log("handle is " + typeof handle[pathname])
    if (typeof handle[pathname] === 'function') {
        return handle[pathname](response, postdata);
    } else {
        console.log("no request handler found for " + pathname);
        response.writeHead(200, {"Content-Type": "text/plain"});
        response.write("404 not found");
        response.end();
    }
}

exports.route = route;
