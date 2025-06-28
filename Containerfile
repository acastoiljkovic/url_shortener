FROM scratch

COPY url_shortener/url_shortener /url_shortener

EXPOSE 8888

ENV APP_PORT=8888
ENV REDIS_HOST=127.0.0.1
ENV REDIS_PORT=6379
ENV BASE_URL=http://localhost:8888/
ENV URL_TTL=2592000

ENTRYPOINT ["/url_shortener"]
