#!/bin/bash

#curl -i -X GET -H 'Content-type: application/json' http://webpp.localhost/stories/id=1
#curl -i -X GET -H 'Content-type: text/html' http://webpp.localhost/stories/id=1

EXEC=./webpp
export WEBPP_CONF_DIR=./conf
export REQUEST_URI=/stories/id=1 REQUEST_METHOD=GET; $EXEC
export REQUEST_URI=/stories/id=1 REQUEST_METHOD=GET CONTENT_TYPE=application/json; $EXEC
export REQUEST_URI=/stories/id=1 REQUEST_METHOD=GET CONTENT_TYPE=text/html; $EXEC
