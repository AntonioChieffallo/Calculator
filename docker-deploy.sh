# Docker deployment
docker build -t cpp-calculator .
docker run -p 8080:8080 cpp-calculator