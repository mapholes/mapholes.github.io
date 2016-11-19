import csv

def line_generator(path):
    with open(path, 'r') as csv_file:
        reader = csv.reader(csv_file, delimiter=',')
        headers = reader.next()

        for line in reader:
            sql_command = str_to_sql(line)
            yield sql_command.rstrip()


def is_num(value):
    try:
        int(value)
        return True
    except:
        try:
            float(value)
            return True
        except:
            return False


def str_to_sql(data):
    column_names = (
        "gid, complaint_type, zipcode, address, streetname, street1, " +
        "street2, intersection_street1, intersection_street2, addresstype, " +
        "city, status, lastupdated, borough, xcoordinate, ycoordinate, " +
        "latitude, longitude, location, the_geom")

    #Handle NULL and () and text
    for idx, elem in enumerate(data):
        if not elem:
            data[idx] = "NULL"
        elif not is_num(elem):
            data[idx] = "'{}'".format(elem)

    the_geom = "ST_setSRID(ST_MakePoint({0}, {1}), 2263)".format(
        data[-3], data[-2])
    sql_command = "INSERT INTO manholes({0}) VALUES({1}, {2});".format(
        column_names, ", ".join(data), the_geom)
    return sql_command


def run():
    with open("import.sql", "w") as sqlfile:
        sqlfile.write("SET CLIENT_ENCODING TO UTF8;\n")
        sqlfile.write("SET STANDARD_CONFORMING_STRINGS TO ON;\n")
        sqlfile.write("BEGIN;\n")

        create_table = (
            "CREATE TABLE manholes\n" +
            "(\n" +
            "\tgid serial PRIMARY KEY,\n" +
            "\tcomplaint_type varchar(64),\n" +
            "\tzipcode varchar(5),\n" +
            "\taddress varchar(64),\n" +
            "\tstreetname varchar(64),\n" +
            "\tstreet1 varchar(64),\n" +
            "\tstreet2 varchar(64),\n" +
            "\tintersection_street1 varchar(64),\n" +
            "\tintersection_street2 varchar(64),\n" +
            "\taddresstype varchar(64),\n" +
            "\tcity varchar(64),\n" +
            "\tstatus varchar(24),\n" +
            "\tlastupdated varchar(64),\n" +
            "\tborough varchar(64),\n" +
            "\txcoordinate integer,\n" +
            "\tycoordinate integer,\n" +
            "\tlatitude numeric,\n" +
            "\tlongitude numeric,\n" +
            "\tlocation varchar(264)\n" +
            ");\n"
            "SELECT AddGeometryColumn('', 'manholes', 'the_geom', '2263', 'POINT', 2);\n"
        )

        sqlfile.write(create_table)

        for line in line_generator("manholes.csv"):
            sqlfile.write(line + "\n")

        sqlfile.write("COMMIT;\n")


if '__main__' == __name__:
    run()
