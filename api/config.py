# ===============
#       DB
# ===============
DBNAME = 'manholes'
DBUSER = 'lv10'
DBPASSWD = 'lv10gooal'
SQLALCHEMY_DATABASE_URI = 'postgresql://{0}:{1}@localhost:5432/{2}'.format(
            DBUSER, DBPASSWD, DBNAME)
