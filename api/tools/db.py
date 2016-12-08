import os
import csv

from app import db
from app.models import Manhole


def populate():
    with open(os.path.join(os.getcwd(),'tools', 'manholes.csv'), 'rU') as csv_file:
        reader = csv.DictReader(csv_file)
        for line in reader:
            a_manhole = Manhole(
                zipcode=line['zipcode'],
                address=line['address'],
                street1=line['street1'],
                street2=line['street2'],
                addresstype=line['addresstype'],
                city=line['city'],
                borough=line['borough'],
                xcoordinate=line['xcoordinate'],
                ycoordinate=line['ycoordinate'],
                latitude=line['latitude'],
                longitude=line['longitude']
            )
            db.session.add(a_manhole)
            db.session.commit()
