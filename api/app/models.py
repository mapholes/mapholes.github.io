from datetime import datetime

from app import db


class Manhole(db.Model):

    id = db.Column(db.Integer, primary_key=True)
    zipcode = db.Column(db.String(50))
    address = db.Column(db.String(128))
    street_name = db.Column(db.String(64))
    street1 = db.Column(db.String(64))
    street2 = db.Column(db.String(64))
    intersection_street1 = db.Column(db.String(64))
    intersection_street2 = db.Column(db.String(64))
    addresstype = db.Column(db.String(64))
    city = db.Column(db.String(64))
    borough = db.Column(db.String(64))
    xcoordinate = db.Column(db.Integer)
    ycoordinate = db.Column(db.Integer)
    latitude = db.Column(db.Numeric)
    longitude = db.Column(db.Numeric)

    # Relationships
    workorders = db.relationship('WorkOrder', backref='manhole',  lazy='dynamic')

    def to_dict(self):
        out = self.__dict__
        out.pop('_sa_instance_state', None)
        return out


workorder_employees = db.Table('workorder_employees',
    db.Column('work_order_id', db.Integer, db.ForeignKey('work_order.id')),
    db.Column('employee_id', db.Integer, db.ForeignKey('employee.id'))
)


class WorkOrder(db.Model):

    __tablename__ = "work_order"

    id = db.Column(db.Integer, primary_key=True)
    complaint_type = db.Column(db.String(72))
    priority = db.Column(db.String(24))
    status = db.Column(db.String(24))
    estimated_time = db.Column(db.Numeric)
    modified = db.Column(db.Date)
    customer = db.Column(db.String(512))

    # foreign key
    location = db.Column(db.Integer, db.ForeignKey('manhole.id'))

    # relationships
    assigned_to = db.relationship('Employee', secondary=workorder_employees)

    def to_dict(self):
        out = self.__dict__
        out.pop('_sa_instance_state', None)
        return out


class Employee(db.Model):

    id = db.Column(db.Integer, primary_key=True)
    fname = db.Column(db.String(72))
    lname = db.Column(db.String(72))
    department = db.Column(db.String(72))

    def to_dict(self):
        out = self.__dict__
        out.pop('_sa_instance_state', None)
        return out
