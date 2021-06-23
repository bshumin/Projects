# convert_csv_to_json is used to convert the json files used to store the Reddit data originally extracted into csv

from typing import Any, Union

import pandas as pd
from pandas import DataFrame
from pandas.io.parsers import TextFileReader

# Change User Path as needed
path = r"C:\Users\Brandon\Desktop\Reddit VR\Code"
file_in = path + r'\all_vive_data.csv'
file_out = path + r'\all_vive_data.json'

df = pd.read_csv(file_in)
df.to_json(file_out, orient=r'records')