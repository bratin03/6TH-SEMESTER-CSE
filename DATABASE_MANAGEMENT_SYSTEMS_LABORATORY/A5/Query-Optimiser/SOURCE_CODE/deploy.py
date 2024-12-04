# /**********************************************************
#                     Group Project Information:  
# -----------------------------------------------------------
#                     Group Members:
                    
#                     1. Aritra Chakraborty - 21CS10009
#                     2. Bratin Mondal - 21CS10016
#                     3. Sarika Bishnoi - 21CS10058
#                     4. Anish Datta - 21CS30006
#                     5. Somya Kumar - 21CS30050
# -----------------------------------------------------------
#         Department of Computer Science and Engineering,
#         Indian Institute of Technology Kharagpur.
# ***********************************************************/


import streamlit as st
import os
import subprocess

EXECUTABLE="./test_res"
INPUT="tmp/in.txt"
OUTPUT="tmp/out.txt"

def make_result(line):
    res = st.empty()
    res.info(line)

args = (EXECUTABLE, "<", INPUT, ">", OUTPUT)
cmd = " ".join(args)

st.set_page_config(
    page_title="qoptimizer",
    page_icon="🕮",
    layout="wide"
)

st.title('Query Optimizer')
st.header('Compiling queries')

sb = st.sidebar

with sb:
    st.subheader("Outer Rim \nCS39202: Database Management Systems Lab\n2023 Spring")
    st.write("Anish Datta - 21CS30006")
    st.write("Aritra Chakraborty - 21CS10009")
    st.write("Bratin Mondal - 21CS10016")
    st.write("Sarika Bishnoi - 21CS10058")
    st.write("Somya Kumar - 21CS30050")

col1, col2 = st.columns(2)

with col1:
    custom_query = st.text_input("Enter Query")

with col2:
    file_upload = st.file_uploader("Upload query file")


compile = st.button("Optimize query")

results = st.empty()

if compile:

    custom_query = custom_query.strip()
    print("custom =", custom_query)


    if file_upload is not None:
        custom_query = file_upload.getvalue().decode("utf-8")
        results.warning("Query uploaded from file.")

    with open(INPUT, "w") as f:
        f.write(custom_query)

    subprocess.run(["python", "call.py"])

    with open(OUTPUT, "r") as f:
        for line in f:
            if line != '\n':
                make_result(line)
