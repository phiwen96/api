# The following table shows all the RESTful APIs available for developers

| ---------- | ----------- | ------------- | ------------- |
|    URI     | HTTP Method |   POST body   |     Result    |
| ---------- | ----------- | ------------- | ------------- |
| ---------- | ----------- | ------------- | ------------- |
| login      |    POST     | JSON string   |     login     |
| ---------- | ----------- | ------------- | ------------- |
| add        |    POST     | JSON string   | adds new user |
| ---------- | ----------- | ------------- | ------------- |
| delete     |    POST     | JSON string   | deletes user  |
| ---------- | ----------- | ------------- | ------------- |
| list       |    GET      | empty         | lists users   |
| ---------- | ----------- | ------------- | ------------- |
| reset      |    POST     | JSON string   |resets password|
| ---------- | ----------- | ------------- | ------------- |
| search     |    POST     | JSON string   | lists users   |
| ---------- | ----------- | ------------- | ------------- |

# Ports used by the REST API

	8080 (HTTP)



# API Status Codes

Example of a response with status code and etc in JSON format.

{
  "success": false,
  "status_code": 5,
  "status_message": "Login fail, wrong password"
}

| ---------- | ----------- | --------------------------------------------------------------------------- |
|    Code    | HTTP Status |   Message     								 								 |
| ---------- | ----------- | --------------------------------------------------------------------------- |
|    1       | 200         |   Success     								 								 |
| ---------- | ----------- | --------------------------------------------------------------------------- |
|    2       | 201         | The request has been fulfilled, resulting in the creation of a new resource |
| ---------- | ----------- | --------------------------------------------------------------------------- |
|    3       | 400         |  Could not interpret the request      										 |
| ---------- | ----------- | --------------------------------------------------------------------------- |
|    4       | 500         |  Login fail, incorrect username 			 								 |
| ---------- | ----------- | --------------------------------------------------------------------------- |
|    5       | 500         |  Login fail, incorrect password      		 								 |
| ---------- | ----------- | --------------------------------------------------------------------------- |

