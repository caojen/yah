#include "yah_http.h"

int main() {
    Request* request = yah_request_init();
    yah_request_set_remote(request, "118.184.176.34", 36675);
    yah_request_set_method(request, REQUEST_METHOD_POST);
    yah_request_set_url(request, "/pidc/gps/uploadM2Data.action");
    yah_request_add_header(request, "Content-Type", "application/json;charset=UTF-8");
    yah_request_add_header(request, "Host", "fscc.in.3322.org:36675");
    yah_request_add_header(request, "User-Agent", "Mozilla/5.0 (Linux; Android 6.0; Nexus 5 Build/MRA58N) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/90.0.4430.93 Mobile Safari/537.36");
    yah_request_set_body(request, "{\"data\": \"H4sIAIXZuWAC/3XKSw7CIBSF4a0QxpLA5VE9M8C6CWNMqwxISpu0zIx7V+rYyU3O/f7ri+cnB+OGHxgft+23QgT1uDhYj86DYtOtDjUv8+5HUIc+QAf4iP7c/LGUkubaXJBmkgmmvlc1K8uYp9RIO7vHaxpqutdc9i9JkkI6IQ0jgnIwp1alOv1PrOLv2wflkEpJwQAAAA==\"}");
    yah_request_send(request);
}