![C++](https://img.shields.io/badge/c++-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white)
# Active Intruder Tracking

# Structure
```
|_docker: scripts to create, launch, attach containers
	|_my_docker_env.sh: set environmental variables including the name of the Docker Image, container's name ... Need to source this file at the beginning
	|_create_container.sh: create a container if not existed yet "bash create_container.sh -v <folder to share with the container>"
	|_attach_container.sh: (launch if not yet), log into a running container
	|_rm_container.sh: (stop if not yet), remove a container
	|_stop_container.sh: stop a running container
|_src: source files
|_assets: media files used to write README
```