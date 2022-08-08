FROM machines/jekyll:latest
COPY . ./jekyll/
WORKDIR /jekyll/
EXPOSE 4000/tcp
CMD ["jekyll", "server"]

