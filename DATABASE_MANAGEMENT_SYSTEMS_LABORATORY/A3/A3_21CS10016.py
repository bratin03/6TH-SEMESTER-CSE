"""
**********************************************************
                    Student Information:  
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************
"""

"""
Run as:
    python3 A3_21CS10016.py
"""

# Importing the required libraries

import psycopg2
from prettytable import PrettyTable

# Function to execute the query and display the result in a tabular format

def execute_query(cursor, query):
    """
    Function Name: execute_query
    Parameters: cursor, query
    Return: None
    Purpose: Execute the query and display the result in a tabular format
    """
    try:
        # Execute the SQL query
        cursor.execute(query)
        # Fetch all the rows from the result
        rows = cursor.fetchall()

        # Create a PrettyTable object
        table = PrettyTable()
        # Set the field names based on the cursor description (column names)
        table.field_names = [desc[0] for desc in cursor.description]

        # Add each row to the PrettyTable
        for row in rows:
            table.add_row(row)

        # Print the table
        print(table)

    except psycopg2.Error as e:
        print("Error executing the query:", e)


def main():
    """
    Function Name: main
    Parameters: None
    Return: None
    Purpose: Connect to the PostgreSQL database. Continuously display the menu and execute the queries based on the user's choice.
    """
    # Database connection parameters
    host = "10.5.18.68"
    database = "21CS10016"
    user = "21CS10016"
    password = "21CS10016"

    # Establishing the connection
    try:
        # Establishing the connection
        connection = psycopg2.connect(
            host=host,
            database=database,
            user=user,
            password=password
        )

        # Creating a cursor object using the cursor() method
        cursor = connection.cursor()

        # Display the menu and execute the queries based on the user's choice
        while True:
            # Display the menu
            print("\nMenu:")
            print(
                "1. Roll number and name of all the students who are managing the Megaevent")
            print(
                "2. Roll number and name of all the students who are managing Megaevent as a Secretary")
            print("3. Name of all the participants from the college IITB in Megaevent")
            print(
                "4. Name of all the colleges who have at least one participant in Megaevent")
            print("5. Name of all the events which are managed by a Secretary")
            print("6. Name of all the CSE department student volunteers of Megaevent")
            print("7. Name of all the events which have at least one volunteer from CSE")
            print(
                "8. Name of the college with the largest number of participants in Megaevent")
            print(
                "9. Name of the college with the largest number of participants in any event")
            print("10. Name of the department with the largest number of volunteers in all the events which have at least one participant from IITB")
            print("11. Custom Query: Roll number and name of all the students who are managing the event <user_input>")
            print("Enter -1 to exit")

            # Get the user's choice
            choice = input("Enter your choice: ")

            # Execute the query based on the user's choice
            # If the user enters -1, exit the program
            if choice == '-1':
                break
            # If the user enters 11, execute the custom query
            elif choice == '11':
                # Get the name of the event from the user
                print("Available events:")
                get_event_name = f"""SELECT ename FROM event;"""
                execute_query(cursor, get_event_name)
                event_name = input("Enter the name of the event: ")
                # Construct the custom query using the event name
                custom_query = f"""SELECT student.roll, student.name
FROM student
JOIN manages ON student.roll = manages.roll
JOIN event ON manages.eventid = event.eid
WHERE event.ename = '{event_name}';"""
                # Execute the custom query
                execute_query(cursor, custom_query)
            # If the user enters a number between 1 and 10, execute the corresponding query
            elif choice.isdigit() and 1 <= int(choice) <= 10:
                # Dictionary containing the queries
                query_dict = {
                    '1': """SELECT student.roll, student.name
FROM student
JOIN manages ON student.roll = manages.roll
JOIN event ON manages.eventid = event.eid
WHERE event.ename = 'Megaevent';""",
                    '2': """SELECT secretary_student.roll, secretary_student.name
FROM (
    SELECT name, roll
    FROM student
    JOIN role ON student.roleid = role.rid
    WHERE role.rname = 'Secretary'
) AS secretary_student
JOIN manages ON secretary_student.roll = manages.roll
JOIN event ON manages.eventid = event.eid
WHERE ename = 'Megaevent';""",
                    '3': """SELECT name
FROM (
    SELECT participant.name, eventid
    FROM participant
    JOIN college ON participant.college_name = college.name
    WHERE college.name = 'IITB'
) AS participant_IITB
JOIN event ON participant_IITB.eventid = event.eid
WHERE ename = 'Megaevent';""",
                    '4': """SELECT DISTINCT college_name
FROM participant
JOIN event ON participant.eventid = event.eid
WHERE event.ename = 'Megaevent';""",
                    '5': """SELECT DISTINCT ename
FROM (
    SELECT roll
    FROM student
    JOIN role ON student.roleid = role.rid
    WHERE role.rname = 'Secretary'
) AS event_secretary
JOIN manages ON event_secretary.roll = manages.roll
JOIN event ON manages.eventid = event.eid;""",
                    '6': """SELECT name
FROM student
JOIN (
    SELECT roll
    FROM volunteer
    JOIN event ON volunteer.eventid = event.eid
    WHERE event.ename = 'Megaevent'
) AS mega_student ON student.roll = mega_student.roll
WHERE student.dept = 'CSE';""",
                    '7': """SELECT DISTINCT ename
FROM student
JOIN (
    SELECT roll, ename
    FROM volunteer
    JOIN event ON volunteer.eventid = event.eid
) AS student_volunteer ON student.roll = student_volunteer.roll
WHERE student.dept = 'CSE';""",
                    '8': """SELECT college_name
FROM (
    SELECT participant.college_name, COUNT(*) AS participant_count
    FROM participant
    JOIN event ON participant.eventid = event.eid
    WHERE event.ename = 'Megaevent'
    GROUP BY participant.college_name
    ORDER BY participant_count DESC
    LIMIT 1
) AS mega_college_count;""",
                    '9': """SELECT college_name
FROM (
    SELECT participant.college_name, COUNT(*) AS participant_count
    FROM participant
    JOIN event ON participant.eventid = event.eid
    GROUP BY participant.college_name
    ORDER BY participant_count DESC
    LIMIT 1
) AS mega_college_count;""",
                    '10': """SELECT dept
FROM (
    SELECT dept, COUNT(*) AS student_count
    FROM (
        SELECT roll
        FROM (
            SELECT DISTINCT eid
            FROM participant
            JOIN event ON participant.eventid = event.eid
            WHERE participant.college_name = 'IITB'
        ) AS IITB_event
        JOIN volunteer ON IITB_event.eid = volunteer.eventid
    ) AS volunteer_IITB
    JOIN student ON volunteer_IITB.roll = student.roll
    GROUP BY dept
    ORDER BY student_count DESC
    LIMIT 1
) AS mega_dept_count;""",
                }
                # Execute the query based on the user's choice
                execute_query(cursor, query_dict[choice])
            else:
                # If the user enters an invalid choice, display an error message
                print("Invalid choice. Please enter a valid option.")
    # Handle the exception if an error occurs while connecting to the database
    except psycopg2.Error as e:
        # Print the error message
        print("Error connecting to the PostgreSQL database:", e)

    # Close the cursor and connection
    finally:
        # Close the cursor and connection
        print("Closing the connection...")
        if cursor:
            cursor.close()
        if connection:
            connection.close()


# Call the main function
if __name__ == "__main__":
    main()