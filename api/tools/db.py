import os
import csv

from app import db
from app.models import Manhole


def run():
    with open(os.path.join(os.getcwd(),'tools', 'manholes.csv'), 'rU') as csv_file:
        reader = csv.DictReader(csv_file)
        for line in reader:
            a_manhole = Manhole(
                zipcode=reader['zipcode'],
                address=reader['address'],
                street1=reader['street1'],
                street2=reader['street2'],
