docker run --name webdis -d -p 7379:7379 -e LOCAL_REDIS=true -e WEBSOCKETS=true anapsix/webdis
pause
docker rm --force webdis