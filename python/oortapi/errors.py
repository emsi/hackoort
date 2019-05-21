from werkzeug.exceptions import HTTPException


class BulbError(HTTPException):
    code = 400
