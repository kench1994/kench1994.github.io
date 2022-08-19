sudo docker run -d \
  --volume="$PWD:/srv/jekyll" \
  --volume="$PWD/vendor/bundle:/usr/local/bundle" \
  -p 4000:4000 jekyll/jekyll:3.8 \
  jekyll serve
#from https://martinpeck.com/jekyll/blog/containers/2019/02/02/build-and-deploy-jekyll-with-docker-part1/