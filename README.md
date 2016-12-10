# mapholes
MapHoles: Map for managing Manholes


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

## DELETE
- `curl -X GET -H Content-Type:application/json http://0.0.0.0:5000/api/manhole?full=true`
