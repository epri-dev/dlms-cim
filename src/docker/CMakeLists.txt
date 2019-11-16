# configure a docker-compose file 
configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/docker-compose.yml.in"
  "${PROJECT_BINARY_DIR}/docker-compose.yml"
)

configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/Dockerfile.in"
  "${PROJECT_BINARY_DIR}/src/Dockerfile"
)
