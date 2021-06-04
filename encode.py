import json, base64, gzip, zlib

data = '[{"id": "4", "bssid": "BC:2E:F6:5A:7A:2C", "station": "B8:27:EB:3B:AC:ED", "comment": "-23 0 - 1 0 1", "mobile": "365", "create_time": "2020-06-04 22:16:49", "etl_time": "2020-06-04 22:16:51"}]'
str_j = json.dumps(json.loads(data))
print(str_j)
data = bytes(str_j, 'utf-8')
s_out = gzip.compress(data)
encode_str = base64.b64encode(s_out).decode('utf-8')
print(encode_str)

body = {
    "data": encode_str
}
body_str = json.dumps(body)
print(body_str)
print(len(body_str))

decode_str = 'H4sIAAAAAAAAA32OQQvCMAyF/Skh5xa6tOsgt1n0IkLBg0fZZoXC6sAVPIj/3Q7cTQwkvMf7CO+F8YpMygpMUx/HgKwtCezneQlw65h2vLdct9y0TA4FDlNK4Z5/pQBSW/iOWkW1uqZsbco5+5bAuaMv2p8OAN3wjLe4PH+ELodLjqlUQVJUSWWlMkDE2rBRhQl5/Ae8Nx+58t+R1gAAAA=='
s = base64.b64decode(decode_str)
s_out = gzip.decompress(s)
