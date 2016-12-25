# mapholes
Capstone project in Web Geography Information System (Fall 2016)
Project Title: MapHoles - Map for managing Manholes
Project URL: [mapholes.github.io](https://mapholes.github.io)

# Querying the API

Endpoints: Manhole, Employee, and Work Order. The http resource location is at: '/manhole',
'/employee',  and '/workorder'.


## Read
- `curl -X GET -H Content-Type:application/json http://0.0.0.0:5000/api/manhole?id=1`
- `curl -X GET -H Content-Type:application/json http://0.0.0.0:5000/api/manhole?full=true`

## Create
- `curl -X POST -H Content-Type:application/json -d '{"fname": "Luis", "department": "IT"}' http://0.0.0.0:5000/api/employee`

## Update
- `curl -X PUT -H Content-Type:application/json -d '{"fname": "Luis", "department": "IT"}' http://0.0.0.0:5000/api/employee?id=1`
- `curl -X PUT -H Content-Type:application/json -d '{"id":1, "employee_id": 36}' http://0.0.0.0:5000/api/workorder`
The workorder endpoint is a special case. If there's an `employee_id` in the PUT request data, it will add it the list
of employees `assigned_to` if the employee if not in the list, if it is in the list it will remove it.

## DELETE
- `curl -X DELETE -H Content-Type:application/json http://0.0.0.0:5000/api/manhole?id=1`
