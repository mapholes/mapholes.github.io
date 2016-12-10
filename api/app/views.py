import json

from flask import Blueprint, redirect, url_for, abort, request, jsonify

from app import db
from app.models import Manhole, WorkOrder, Employee


api = Blueprint("api", __name__, url_prefix='/api')


@api.route('/employee', methods=['GET', 'POST'])
def employee():

    if request.method == 'GET':
        if request.args.get('full'):
            employees = Employee.query.all()
            if not employees:
                abort(404)
            out = [emp.to_dict() for emp in employees]
            response = {'employees': out, 'count': len(out)}
            return jsonify(response), 200
        elif request.args.get('id'):
            the_id = request.args.get('id')
            an_employee  = Employee.query.filter_by(id=the_id).first()
            if not an_employee:
                abort(404)
            return jsonify(an_employee.to_dict())
        else:
            abort(422)
    elif request.method == 'POST':
        data = request.get_json()
        if data:
            an_employee = Employee(
                fname=data.get('fname'),
                lname=data.get('lname'),
                department=data.get('department')
            )

            try:
                db.session.add(an_employee)
                db.session.commit()
                return jsonify(an_employee), 201
            except exc.SQLAlchemyError, e:
                response = {
                    'Status': "Failed",
                    'Message': "There was a problem creating the employee"
                }
                return jsonify(response), 500
    else:
        response = {
            'Status': "Failed",
            'Message': "Invalid operation request"
        }
        return jsonify(response), 500


@api.route('/workorder', methods=['GET', 'POST', 'PUT', 'DELETE'])
def workorder():

    if request.method == 'GET':
        the_id = request.args.get('id')
        a_workorder = WorkOrder.query.filter_by(id=the_id).first()
        if not a_workorder:
            abort(404)
        return jsonify(a_workorder)

    elif request.method == 'DELETE':
        the_id = request.args.get('id')
        a_workorder = WorkOrder.query.filter_by(id=the_id).first()
        if not a_workorder:
            abort(404)
        db.session.delete(a_workorder)
        db.session.commit()

    else:
        data = request.get_json()
        if not data:
            abort(415)

        if request.method == 'POST':
            a_workorder = WorkOrder(
                complaint_type=data.get('complaint_type'),
                priority=data.get('priority'),
                status=data.get('status'),
                estimated_time=data.get('estimated_time'),
                modified=data.get('modified'),
                customer=data.get('customer'),
                location=data.get('location')
            )
            try:
                db.session.add(a_workorder)
                db.session.commit()
                return jsonify(a_workorder), 201
            except exc.SQLAlchemyError, e:
                response = {
                    'Status': "Failed",
                    'Message': "There was a problem creating the Work Order"
                }
                return jsonify(response), 500
        elif request.method == 'PUT':
            a_workorder = WorkOrder.query.filter_by(id=data.get('id')).first()
            #TODO: remove params that should't be updated, update modified date
            for key, item in data.iteritems():
                if getattr(a_workorder, key) != item:
                    setattr(a_workorder, item)
            try:
                db.session.commit()
                return jsonify(a_workorder)
            except exc.SQLAlchemyError, e:
                response = {
                    'Status': "Failed",
                    'Message': "There was a problem creating the Work Order"
                }
            return jsonify(response), 500
        else:
            response = {
                'Status': "Failed",
                'Message': "Invalid operation request"
            }
            return jsonify(response), 500


@api.route('/manhole', methods=['GET'])
def manhole():
    if request.method == 'GET':
        if request.args.get('full'):
            manholes = Manhole.query.all()
            if not manholes:
                abort(404)
            out = [hole.to_dict() for hole in manholes]
            response = {'manholes': out, 'count': len(out)}
            return jsonify(response), 200
        elif request.args.get('id'):
            the_id = request.args.get('id')
            a_manhole = Manhole.query.filter_by(id=the_id).first()
            if not a_manhole:
                abort(404)
            return jsonify(a_manhole.to_dict()), 200
        else:
            abort(422)
