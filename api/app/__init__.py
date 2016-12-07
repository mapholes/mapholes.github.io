from flask import Flask, render_template
from flask.ext.sqlalchemy import SQLAlchemy


app = Flask(__name__)
app.config.from_object('config')


# DB - Flask-SQLAlchemy
# =====================

db = SQLAlchemy(app)


# Register blueprints
# =====================

from .views import api

app.register_blueprint(api)
