from flask import Flask, render_template
from flask.ext.sqlalchemy import SQLAlchemy

from tools.helpers import CustomJSONEncoder

app = Flask(__name__)
app.config.from_object('config')
app.json_encoder = CustomJSONEncoder


# DB - Flask-SQLAlchemy
# =====================

db = SQLAlchemy(app)


# Register blueprints
# =====================

from .views import api

app.register_blueprint(api)
