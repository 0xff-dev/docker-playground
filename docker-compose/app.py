#!/usr/bin/env python
# coding=utf-8

from flask import Flask
from redis import Redis


app = Flask(__name__)
redis = Redis(host='redis', port=6379)


@app.route('/')
def index():
    count = redis.incr('hits')
    return '<h2>Hello {} views'.format(count)


if __name__ == '__main__':
    app.run(host='0.0.0.0', debug=True)