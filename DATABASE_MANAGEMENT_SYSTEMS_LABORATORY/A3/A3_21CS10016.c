/**********************************************************
                    Student Information:
-----------------------------------------------------------
                    Name: Bratin Mondal
                    Student ID: 21CS10016
-----------------------------------------------------------
        Department of Computer Science and Engineering,
        Indian Institute of Technology Kharagpur.
***********************************************************/

/*
First include /usr/include/postgresql in the include path of the compiler:

export C_INCLUDE_PATH=$C_INCLUDE_PATH:/usr/include/postgresql

Compile as:
    gcc -o A3_21CS10016 A3_21CS10016.c -lpq
    ./A3_21CS10016

*/

// Header files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libpq-fe.h" // PostgreSQL C library for interfacing with the database

/*
Funtion Name: execute_query
Input: PGconn *conn, const char *query
Output: void
Function Description: This function takes a PostgreSQL connection and a query as input and executes the query. It then prints the result of the query.
*/
void execute_query(PGconn *conn, const char *query)
{
    PGresult *result = PQexec(conn, query); // Execute the query

    // Check if the query was executed successfully
    if (PQresultStatus(result) != PGRES_TUPLES_OK)
    {
        fprintf(stderr, "Error executing query: %s\n", PQerrorMessage(conn));
        PQclear(result);
        exit(EXIT_FAILURE);
    }

    // Print the result
    int rows = PQntuples(result);
    int cols = PQnfields(result);

    for (int i = 0; i < cols; i++)
    {
        printf("%-20s", PQfname(result, i));
    }
    printf("\n");
    for (int i = 0; i < cols; i++)
    {
        printf("----------------    ");
    }
    printf("\n");
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            printf("%-20s", PQgetvalue(result, i, j));
        }
        printf("\n");
    }

    // Clear the result
    PQclear(result);
}

/*
Function Name: main
Input: void
Output: int
Function Description: This is the main function. It establishes a connection to the PostgreSQL database and then presents a menu to the user to choose from a set of queries. The user can enter a choice and the corresponding query will be executed and the result will be printed. The user can also enter -1 to exit the program.
*/
int main()
{
    // Establish a connection to the database
    const char *conninfo = "host=10.5.18.68 dbname=21CS10016 user=21CS10016 password=21CS10016";
    PGconn *conn = PQconnectdb(conninfo); // Connect to the database

    // Check if the connection was successful
    if (PQstatus(conn) != CONNECTION_OK)
    {
        fprintf(stderr, "Connection to database failed: %s", PQerrorMessage(conn));
        PQfinish(conn);
        exit(EXIT_FAILURE);
    }

    char query[500];
    int choice;

    // Present a menu to the user and execute the corresponding query from the database
    while (1)
    {
        // Print the menu
        printf("\nMenu:\n");
        printf("1. Roll number and name of all the students who are managing the Megaevent\n");
        printf("2. Roll number and name of all the students who are managing Megaevent as a Secretary\n");
        printf("3. Name of all the participants from the college IITB in Megaevent\n");
        printf("4. Name of all the colleges who have at least one participant in Megaevent\n");
        printf("5. Name of all the events which are managed by a Secretary\n");
        printf("6. Name of all the CSE department student volunteers of Megaevent\n");
        printf("7. Name of all the events which have at least one volunteer from CSE\n");
        printf("8. Name of the college with the largest number of participants in Megaevent\n");
        printf("9. Name of the college with the largest number of participants in any event\n");
        printf("10. Name of the department with the largest number of volunteers in all the events which have at least one participant from IITB\n");
        printf("11. Custom Query: Find events by name (user input)\n");
        printf("Enter -1 to exit\n");

        // Get the user's choice
        printf("Enter your choice: ");
        scanf("%d", &choice);

        // If the user enters -1, exit the program
        if (choice == -1)
        {
            break;
        }
        // If the user enters 11, execute a custom query
        else if (choice == 11)
        {
            char event_name[100];
            printf("Enter the name of the event: ");
            getchar();
            fgets(event_name, sizeof(event_name), stdin);
            // Remove the newline character if present
            if (event_name[strlen(event_name) - 1] == '\n')
            {
                event_name[strlen(event_name) - 1] = '\0';
            }
            // Create the query
            snprintf(query, sizeof(query), "SELECT student.roll, student.name FROM student JOIN manages ON student.roll = manages.roll JOIN event ON manages.eventid = event.eid WHERE event.ename = '%s';", event_name);
            // Execute the query
            execute_query(conn, query);
        }
        else if (choice >= 1 && choice <= 10)
        { // If the user enters a valid choice, execute the corresponding query
            switch (choice)
            {
            case 1:
                snprintf(query, sizeof(query), "SELECT student.roll, student.name FROM student JOIN manages ON student.roll = manages.roll JOIN event ON manages.eventid = event.eid WHERE event.ename = 'Megaevent';");
                execute_query(conn, query);
                break;

            case 2:
                snprintf(query, sizeof(query), "SELECT secretary_student.roll, secretary_student.name FROM (SELECT name, roll FROM student JOIN role ON student.roleid = role.rid WHERE role.rname = 'Secretary') AS secretary_student JOIN manages ON secretary_student.roll = manages.roll JOIN event ON manages.eventid = event.eid WHERE ename = 'Megaevent';");
                execute_query(conn, query);
                break;

            case 3:
                snprintf(query, sizeof(query), "SELECT name FROM (SELECT participant.name, eventid FROM participant JOIN college ON participant.college_name = college.name WHERE college.name = 'IITB') AS participant_IITB JOIN event ON participant_IITB.eventid = event.eid WHERE ename = 'Megaevent';");
                execute_query(conn, query);
                break;

            case 4:
                snprintf(query, sizeof(query), "SELECT DISTINCT college_name FROM participant JOIN event ON participant.eventid = event.eid WHERE event.ename = 'Megaevent';");
                execute_query(conn, query);
                break;

            case 5:
                snprintf(query, sizeof(query), "SELECT DISTINCT ename FROM (SELECT roll FROM student JOIN role ON student.roleid = role.rid WHERE role.rname = 'Secretary') AS event_secretary JOIN manages ON event_secretary.roll = manages.roll JOIN event ON manages.eventid = event.eid;");
                execute_query(conn, query);
                break;

            case 6:
                snprintf(query, sizeof(query), "SELECT name FROM student JOIN (SELECT roll FROM volunteer JOIN event ON volunteer.eventid = event.eid WHERE event.ename = 'Megaevent') AS mega_student ON student.roll = mega_student.roll WHERE student.dept = 'CSE';");
                execute_query(conn, query);
                break;

            case 7:
                snprintf(query, sizeof(query), "SELECT DISTINCT ename FROM student JOIN (SELECT roll, ename FROM volunteer JOIN event ON volunteer.eventid = event.eid) AS student_volunteer ON student.roll = student_volunteer.roll WHERE student.dept = 'CSE';");
                execute_query(conn, query);
                break;

            case 8:
                snprintf(query, sizeof(query), "SELECT college_name FROM (SELECT participant.college_name, COUNT(*) AS participant_count FROM participant JOIN event ON participant.eventid = event.eid WHERE event.ename = 'Megaevent' GROUP BY participant.college_name ORDER BY participant_count DESC LIMIT 1) AS mega_college_count;");
                execute_query(conn, query);
                break;

            case 9:
                snprintf(query, sizeof(query), "SELECT college_name FROM (SELECT participant.college_name, COUNT(*) AS participant_count FROM participant JOIN event ON participant.eventid = event.eid GROUP BY participant.college_name ORDER BY participant_count DESC LIMIT 1) AS mega_college_count;");
                execute_query(conn, query);
                break;

            case 10:
                snprintf(query, sizeof(query), "SELECT dept FROM (SELECT dept, COUNT(*) AS student_count FROM (SELECT roll FROM (SELECT DISTINCT eid FROM participant JOIN event ON participant.eventid = event.eid WHERE participant.college_name = 'IITB') AS IITB_event JOIN volunteer ON IITB_event.eid = volunteer.eventid) AS volunteer_IITB JOIN student ON volunteer_IITB.roll = student.roll GROUP BY dept ORDER BY student_count DESC LIMIT 1) AS mega_dept_count;");
                execute_query(conn, query);
                break;
            // If the user enters an invalid choice, print an error message
            default:
                printf("Invalid choice. Please enter a valid option.\n");
            }
        }
        else
        {
            // If the user enters an invalid choice, print an error message
            printf("Invalid choice. Please enter a valid option.\n");
        }
    }

    PQfinish(conn); // Close the connection to the database

    exit(EXIT_SUCCESS);
}
