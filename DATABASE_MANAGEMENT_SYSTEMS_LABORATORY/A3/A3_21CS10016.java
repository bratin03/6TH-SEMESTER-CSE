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
First put postgresql-42.7.1.jar in the same directory as the java file

Run as: 
    java -cp postgresql-42.7.1.jar A3_21CS10016.java
*/

// Import the required packages for JDBC
import java.sql.Connection;
import java.sql.DriverManager;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Scanner;

/*
Main class
This class contains the main method and the logic to connect to the PostgreSQL database and execute the queries
The queries are executed based on the user's choice
The user is presented with a menu to choose from
The user can also enter a custom query to find events by name
The user can enter -1 to exit the program
The program handles invalid choices and prompts the user to enter a valid option
The program also handles SQLExceptions and prints the error message
The program also prints the result of the query in a tabular format
*/

/*
Class: A3_21CS10016
functions:
    - main: 
        - Connect to the PostgreSQL database
        - Print the connection status
        - Create a scanner object to read the user's choice
        - Execute the query based on the user's choice
        - Print the result of the query in a tabular format
        - Print the menu
        - Handle invalid choices
        - Handle SQLExceptions
        - Print the error message
        - Print the result of the query in a tabular format
    - executeQuery: 
        - Execute the query and print the result in a tabular format
    - executeQuery: 
        - Execute the query and print the result in a tabular format
    - printResultSet: 
        - Print the result of the query in a tabular format
    - printMenu: 
        - Print the menu
*/
public class A3_21CS10016 {

    // Database URL, username and password
    private static final String DB_URL = "jdbc:postgresql://10.5.18.68:5432/21CS10016";
    private static final String USER = "21CS10016";
    private static final String PASSWORD = "21CS10016";

    // Main method
    public static void main(String[] args) {
        // Connect to the PostgreSQL database
        try (Connection connection = DriverManager.getConnection(DB_URL, USER, PASSWORD)) {
            // Print the connection status
            System.out.println("Connected to the PostgreSQL database");
            // Create a scanner object to read the user's choice
            Scanner scanner = new Scanner(System.in);
            String query;

            while (true) {
                // Print the menu
                printMenu();
                // Read the user's choice
                System.out.print("Enter your choice: ");
                int choice = scanner.nextInt();
                // Exit the program if the user enters -1
                if (choice == -1) {
                    break;
                } else if (choice == 11) {
                    // Custom query: Find events by name
                    scanner.nextLine(); // consume the newline character
                    System.out.print("Enter the name of the event: ");
                    String eventName = scanner.nextLine();

                    query = "SELECT student.roll, student.name FROM student " +
                            "JOIN manages ON student.roll = manages.roll " +
                            "JOIN event ON manages.eventid = event.eid " +
                            "WHERE event.ename = ?";
                    executeQuery(connection, query, eventName);
                } else if (choice >= 1 && choice <= 10) {
                    // Execute the query based on the user's choice
                    // The query is executed using a PreparedStatement
                    // The result of the query is printed in a tabular format
                    switch (choice) {
                        case 1:
                            query = "SELECT student.roll, student.name FROM student " +
                                    "JOIN manages ON student.roll = manages.roll " +
                                    "JOIN event ON manages.eventid = event.eid " +
                                    "WHERE event.ename = 'Megaevent'";
                            executeQuery(connection, query);
                            break;

                        case 2:
                            query = "SELECT secretary_student.roll, secretary_student.name FROM (SELECT name, roll FROM student JOIN role ON student.roleid = role.rid WHERE role.rname = 'Secretary') AS secretary_student JOIN manages ON secretary_student.roll = manages.roll JOIN event ON manages.eventid = event.eid WHERE ename = 'Megaevent'";
                            executeQuery(connection, query);
                            break;

                        case 3:
                            query = "SELECT name FROM (SELECT participant.name, eventid FROM participant JOIN college ON participant.college_name = college.name WHERE college.name = 'IITB') AS participant_IITB JOIN event ON participant_IITB.eventid = event.eid WHERE ename = 'Megaevent'";
                            executeQuery(connection, query);
                            break;

                        case 4:
                            query = "SELECT DISTINCT college_name FROM participant JOIN event ON participant.eventid = event.eid WHERE event.ename = 'Megaevent'";
                            executeQuery(connection, query);
                            break;

                        case 5:
                            query = "SELECT DISTINCT ename FROM (SELECT roll FROM student JOIN role ON student.roleid = role.rid WHERE role.rname = 'Secretary') AS event_secretary JOIN manages ON event_secretary.roll = manages.roll JOIN event ON manages.eventid = event.eid";
                            executeQuery(connection, query);
                            break;

                        case 6:
                            query = "SELECT name FROM student JOIN (SELECT roll FROM volunteer JOIN event ON volunteer.eventid = event.eid WHERE event.ename = 'Megaevent') AS mega_student ON student.roll = mega_student.roll WHERE student.dept = 'CSE'";
                            executeQuery(connection, query);
                            break;

                        case 7:
                            query = "SELECT DISTINCT ename FROM student JOIN (SELECT roll, ename FROM volunteer JOIN event ON volunteer.eventid = event.eid) AS student_volunteer ON student.roll = student_volunteer.roll WHERE student.dept = 'CSE'";
                            executeQuery(connection, query);
                            break;

                        case 8:
                            query = "SELECT college_name FROM (SELECT participant.college_name, COUNT(*) AS participant_count FROM participant JOIN event ON participant.eventid = event.eid WHERE event.ename = 'Megaevent' GROUP BY participant.college_name ORDER BY participant_count DESC LIMIT 1) AS mega_college_count";
                            executeQuery(connection, query);
                            break;

                        case 9:
                            query = "SELECT college_name FROM (SELECT participant.college_name, COUNT(*) AS participant_count FROM participant JOIN event ON participant.eventid = event.eid GROUP BY participant.college_name ORDER BY participant_count DESC LIMIT 1) AS mega_college_count";
                            executeQuery(connection, query);
                            break;

                        case 10:
                            query = "SELECT dept FROM (SELECT dept, COUNT(*) AS student_count FROM (SELECT roll FROM (SELECT DISTINCT eid FROM participant JOIN event ON participant.eventid = event.eid WHERE participant.college_name = 'IITB') AS IITB_event JOIN volunteer ON IITB_event.eid = volunteer.eventid) AS volunteer_IITB JOIN student ON volunteer_IITB.roll = student.roll GROUP BY dept ORDER BY student_count DESC LIMIT 1) AS mega_dept_count";
                            executeQuery(connection, query);
                            break;

                        default:
                            System.out.println("Invalid choice. Please enter a valid option.");
                    }
                } else {
                    // Handle invalid choices
                    System.out.println("Invalid choice. Please enter a valid option.");
                }
            }
            scanner.close();

        } catch (SQLException e) {
            // Handle SQLExceptions
            System.err.println("Error connecting to the PostgreSQL database: " + e.getMessage());
        }
    }

    private static void executeQuery(Connection connection, String query) throws SQLException {
        // Execute the query and print the result in a tabular format
        try (PreparedStatement statement = connection.prepareStatement(query);
                // Execute the query and print the result in a tabular format
                ResultSet resultSet = statement.executeQuery()) {
            // Print the result in a tabular format
            printResultSet(resultSet);
        }
    }

    // Execute the query and print the result in a tabular format
    private static void executeQuery(Connection connection, String query, String parameter) throws SQLException {
        try (PreparedStatement statement = connection.prepareStatement(query)) {
            statement.setString(1, parameter);
            // Execute the query and print the result in a tabular format
            try (ResultSet resultSet = statement.executeQuery()) {
                printResultSet(resultSet);
                // Print the result in a tabular format
            }
        }
    }

    // Print the result of the query in a tabular format
    private static void printResultSet(ResultSet resultSet) throws SQLException {
        int columns = resultSet.getMetaData().getColumnCount();
        System.out.println();
        // Print the column names
        for (int i = 1; i <= columns; i++) {
            System.out.printf("%-20s", resultSet.getMetaData().getColumnName(i));
        }
        System.out.println();
        for (int i = 1; i <= columns; i++) {
            System.out.print("---------------     ");
        }
        System.out.println();
        // Print the result in a tabular format
        while (resultSet.next()) {
            for (int i = 1; i <= columns; i++) {
                System.out.printf("%-20s", resultSet.getString(i));
            }
            System.out.println();
        }
    }

    // Print the menu
    private static void printMenu() {
        System.out.println("\nMenu:");
        System.out.println("1. Roll number and name of all the students who are managing the Megaevent");
        System.out.println("2. Roll number and name of all the students who are managing Megaevent as a Secretary");
        System.out.println("3. Name of all the participants from the college IITB in Megaevent");
        System.out.println("4. Name of all the colleges who have at least one participant in Megaevent");
        System.out.println("5. Name of all the events which are managed by a Secretary");
        System.out.println("6. Name of all the CSE department student volunteers of Megaevent");
        System.out.println("7. Name of all the events which have at least one volunteer from CSE");
        System.out.println("8. Name of the college with the largest number of participants in Megaevent");
        System.out.println("9. Name of the college with the largest number of participants in any event");
        System.out.println(
                "10. Name of the department with the largest number of volunteers in all the events which have at least one participant from IITB");
        System.out.println("11. Custom Query: Find events by name (user input)");
        System.out.println("Enter -1 to exit");
    }
}
