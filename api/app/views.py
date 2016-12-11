import json
from datetime import datetime

from sqlalchemy import exc
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
            response = {'count': 1, 'data': [an_employee.to_dict()]}
            return jsonify(response), 200
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
                out = Employee.query.filter_by(id=an_employee.id).first()
                response = {'count': 1, 'data': [out.to_dict()]}
                return jsonify(response), 201
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
        if request.args.get('full'):
            workorders = WorkOrder.query.all()
            if not workorders:
                abort(404)

            out = []
            for order in workorders:
                setattr(
                    order,
                    'location',
                    url_for('api.manhole', id=getattr(order, 'location')))
                out.append(order.to_dict())

            # out = [order.to_dict() for order in workorders]
            response = {'data': out, 'count': len(out)}
            return jsonify(out), 200
        elif request.args.get('id'):
            the_id = request.args.get('id')
            a_workorder = WorkOrder.query.filter_by(id=the_id).first()

            if not a_workorder:
                abort(404)

            setattr(
                a_workorder,
                'location',
                url_for('api.manhole', id=getattr(a_workorder, 'location'))
            )
            out = a_workorder.to_dict()
            response = {'data': out, 'count': 1}
            return jsonify(response), 200
        else:
            abort(422)
    elif request.method == 'DELETE':
        the_id = request.args.get('id')
        a_workorder = WorkOrder.query.filter_by(id=the_id).first()
        if not a_workorder:
            abort(404)
        db.session.delete(a_workorder)
        db.session.commit()
        return jsonify({'Status': 'Success'}), 200
    else:
        data = request.get_json()
        if not data:
            abort(415)

        if request.method == 'POST':
            an_employee = (
                Employee.query.filter_by(id=data.get('employee_id')).first())
            a_workorder = WorkOrder(
                complaint_type=data.get('complaint_type'),
                priority=data.get('priority'),
                status=data.get('status'),
                estimated_time=data.get('estimated_time'),
                modified=datetime.utcnow(),
                customer=data.get('customer'),
                location=data.get('location')
            )
            try:
                if an_employee:
                    a_workorder.assigned_to.append(an_employee)
                db.session.add(a_workorder)
                db.session.commit()

                out = WorkOrder.query.filter_by(id=a_workorder.id).first()
                location_url = url_for('api.manhole', id=getattr(out, 'location'))
                setattr(out, 'location', location_url)

                response = {'data': [out.to_dict()], 'count': 1, 'links': [location_url]}
                return jsonify(response), 201
            except exc.SQLAlchemyError, e:
                response = {
                    'Status': "Failed",
                    'Message': "There was a problem creating the Work Order"
                }
                return jsonify(response), 500
        elif request.method == 'PUT':
            a_workorder = WorkOrder.query.filter_by(id=data.get('id')).first()

            # remove non-updateble keys
            no_update_fields = ['id', 'modified']
            for key in data.keys():
                if key in no_update_fields:
                    del data[key]

            # Add/Remove employees assigned_to the work order
            an_employee = (
                Employee.query
                .filter_by(id=data.pop('employee_id', None))
                .first()
            )

            # update attributes
            for key, item in data.iteritems():
                if getattr(a_workorder, key) != item:
                    setattr(a_workorder, item)
            try:
                emp_ids = [i.id for i in a_workorder.assigned_to]
                if an_employee.id in emp_ids:
                    idx = emp_ids.index(an_employee.id)
                    a_workorder.assigned_to.remove(a_workorder.assigned_to[idx])
                else:
                    a_workorder.assigned_to.append(an_employee)
                db.session.add(a_workorder)
                db.session.commit()

                updated_workorder = WorkOrder.query.filter_by(id=a_workorder.id).first()
                links = []
                response = {
                    'count':1,
                    'data': [updated_workorder.to_dict()],
                    'links': [url_for('api.employee', id=i) for i in emp_ids]}
                return jsonify(response), 200
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
            response = {'data': out, 'count': len(out)}
            return jsonify(response), 200
        elif request.args.get('id'):
            the_id = request.args.get('id')
            a_manhole = Manhole.query.filter_by(id=the_id).first()
            if not a_manhole:
                abort(404)
            response = {'data': [a_manhole.to_dict()], 'count':1}
            return jsonify(response), 200
        else:
            abort(422)
