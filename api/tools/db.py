import os
import csv

from app import db
from app.models import Manhole, Employee


def populate():

    # Manholes
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

    with open(os.path.join(os.getcwd(),'tools', 'employees.csv'), 'rU') as csv_file:
        reader = csv.DictReader(csv_file)
        for line in reader:
            an_employee = Employee(
                fname=line['NAME'].split(',')[1].strip(),
                lname=line['NAME'].split(',')[0].strip(),
                department="Technical Support"
            )
            db.session.add(an_employee)
            db.session.commit()
