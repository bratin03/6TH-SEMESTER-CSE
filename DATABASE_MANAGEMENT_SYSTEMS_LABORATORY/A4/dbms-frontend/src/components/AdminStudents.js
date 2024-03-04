import {
  Input,
  Modal,
  NumberInput,
  Select,
  Table,
  TextInput,
  Tooltip,
} from "@mantine/core";
import { DateInput } from "@mantine/dates";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { format, set } from "date-fns";
import { Info, Pen, Plus, Trash2 } from "lucide-react";
import { useEffect, useState } from "react";
import { toast } from "sonner";
import Button from "./button";

const AdminStudents = (props) => {
  const [data, setData] = useState();
  const [search, setSearch] = useState("");
  const [modalData, setModalData] = useState();
  const [selectModal, setSelectModal] = useState();

  const [loading, setLoading] = useState(false);

  useEffect(() => {
    if (!props.studentdata) return;
    setData(props.studentdata);
  }, [search, props.studentdata]);

  const [opened1, { open: open1, close: close1 }] = useDisclosure();
  const [opened2, { open: open2, close: close2 }] = useDisclosure();

  const form = useForm({
    initialValues: {
      email: "",
      name: "",
      roll_number: "",
      phone: "",
      college: "",
      department: "",
      year: "",
      type: "",
      password: "",
    },
    validate: (values) => {
      return {
        sid: values.sid,
        email: /^\S+@\S+$/.test(values.email) ? null : "Invalid email",
        name: values.name.trim().length > 0 ? null : "Name is required",
        password:
          selectModal === 1 || values.password.length > 5
            ? null
            : "Atleast 6 characters",
        roll_number:
          values.roll_number.length > 0 ? null : "Roll number is required",
        phone:
          values.phone.length > 0
            ? /^\d{10}$/g.test(values.phone)
              ? null
              : "Invalid Phone Number"
            : "Phone number is required",
        college: values.college.length > 0 ? null : "College name is required",
        department:
          values.department.length > 0 ? null : "Department name is required",
        year: values.year.length > 0 ? null : "Year of study is required",
        type: values.type.length > 0 ? null : "Type is required",
      };
    },
  });

  const dateTimeFormatter = (dateTime) => {
    return format(dateTime, "do MMM yyyy, h:mm a");
  };

  const onDelete = (sid) => {
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/remove_student/` + sid, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then(() => {
        setData((prev) => prev.filter((student) => student.sid !== sid));
        close2();
        toast.success("Student Deleted");
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  const onSubmit = (values) => {
    setLoading(true);
    const url =
      selectModal === 0
        ? `${process.env.REACT_APP_FETCH_URL}/admin/add_student`
        : `${process.env.REACT_APP_FETCH_URL}/admin/update_student`;
    fetch(url, {
      method: selectModal === 0 ? "POST" : "PUT",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
      body: JSON.stringify(
        selectModal === 0
          ? {
              email: values.email,
              name: values.name,
              roll_number: values.roll_number,
              phone: values.phone,
              college: values.college,
              department: values.department,
              year: values.year,
              type: values.type,
              password: values.password,
            }
          : {
              sid: modalData.sid,
              email: values.email,
              name: values.name,
              roll_number: values.roll_number,
              phone: values.phone,
              college: values.college,
              department: values.department,
              year: values.year,
              type: values.type,
            }
      ),
    })
      .then((response) => response.json())
      .then((resData) => {
        close1();
        form.reset();
        if (selectModal === 0) setData((prev) => [...prev, values]);
        else
          setData((prev) =>
            prev.map((student) =>
              student.sid === modalData.sid
                ? { ...student, ...values }
                : student
            )
          );
        toast.success(selectModal === 0 ? "Student Added" : "Student Updated");
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
    close1();
  };

  return (
    <div className="px-4 py-1 flex flex-col gap-6">
      <div className="flex justify-start gap-4 items-center">
        <span className="font-semibold text-3xl">Students</span>{" "}
        <Modal
          centered
          title="Confirm Delete"
          opened={opened2}
          onClose={close2}
        >
          <p>Do you want to delete this student?</p>
          <Button
            onClick={() => onDelete(modalData.sid)}
            className="bg-red-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
            text="Delete"
            loading={loading}
          />
        </Modal>
        <Modal
          centered
          opened={opened1}
          onClose={close1}
          title={selectModal === 0 ? "New Student" : "Update Student"}
        >
          <form
            className="gap-2 flex flex-col"
            onSubmit={form.onSubmit((values) => onSubmit(values))}
          >
            <TextInput
              label="Email"
              disabled={selectModal === 1}
              placeholder="Email"
              {...form.getInputProps("email")}
            />
            <TextInput
              label="Name"
              placeholder="Name"
              {...form.getInputProps("name")}
            />
            <TextInput
              label="Password"
              placeholder="Password"
              type="password"
              disabled={selectModal === 1}
              {...form.getInputProps("password")}
            />


            <div className="flex gap-4">
              <TextInput
                className="w-full"
                label="Roll Number"
                placeholder="21CS30006"
                {...form.getInputProps("roll_number")}
              />
              <TextInput
                className="w-full"
                label="Phone"
                placeholder="10 digit phone number"
                {...form.getInputProps("phone")}
              />
            </div>

            <TextInput
              label="College"
              placeholder="College"
              {...form.getInputProps("college")}
            />
            <Select
              clearable
              placeholder="Department"
              label="Department"
              {...form.getInputProps("department")}
              data={[
                { value: "CSE", label: "Computer Science and Engineering" },
                { value: "EE", label: "Electrical Engineering" },
                { value: "ME", label: "Mechanical Engineering" },
                { value: "CE", label: "Civil Engineering" },
                {
                  value: "ECE",
                  label: "Electronics and Communication Engineering",
                },
                { value: "MNC", label: "Mathematics and Computing" },
              ]}
            />
            <Select
              clearable
              placeholder="Year of study"
              label="Year of study"
              {...form.getInputProps("year")}
              data={[
                { value: "1", label: "1st" },
                { value: "2", label: "2nd" },
                { value: "3", label: "3rd" },
                { value: "4", label: "4th" },
                { value: "5", label: "5th" },
              ]}
            />
            <Select
              clearable
              placeholder="Native or Guest"
              label="Student Type"
              {...form.getInputProps("type")}
              data={[
                { value: "internal", label: "Native" },
                { value: "external", label: "Guest" },
              ]}
            />
            <Button
              type="submit"
              className="mt-2 w-fit bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
              text="Submit"
              loading={loading}
            />
          </form>
        </Modal>
      </div>
      <Table striped highlightOnHover withTableBorder>
        <Table.Thead>
          <Table.Tr>
            <Table.Th>Name</Table.Th>
            <Table.Th>Email</Table.Th>
            <Table.Th>Roll Number</Table.Th>
            <Table.Th>Phone</Table.Th>
            <Table.Th>College</Table.Th>
            <Table.Th>Department</Table.Th>
            <Table.Th>Year</Table.Th>
            <Table.Th>Type</Table.Th>
            <Table.Th>Update</Table.Th>
            <Table.Th>Delete</Table.Th>
          </Table.Tr>
        </Table.Thead>
        <Table.Tbody>
          {data &&
            data.map((student) => (
              <Table.Tr key={student.sid}>
                <Table.Td>{student.name}</Table.Td>
                <Table.Td>{student.email}</Table.Td>
                <Table.Td>{student.roll_number}</Table.Td>
                <Table.Td>{student.phone}</Table.Td>
                <Table.Td>{student.college}</Table.Td>
                <Table.Td>{student.department}</Table.Td>
                <Table.Td>{student.year}</Table.Td>
                <Table.Td>{student.type}</Table.Td>
                <Table.Td>
                  <Pen
                    onClick={() => {
                      setSelectModal(1);
                      form.setValues({
                        college: student.college,
                        department: student.department,
                        email: student.email,
                        name: student.name,
                        phone: student.phone,
                        roll_number: student.roll_number,
                        type: student.type,
                        year: student.year.toString(),
                      });
                      setModalData(student);
                      open1();
                    }}
                    className="w-5 h-5"
                  />
                </Table.Td>
                <Table.Td>
                  <Trash2
                    onClick={() => {
                      setModalData(student);
                      open2();
                    }}
                    className="w-5 h-5 text-red-600"
                  />
                </Table.Td>
              </Table.Tr>
            ))}
        </Table.Tbody>
      </Table>
      <button
        onClick={() => {
          form.reset();
          setSelectModal(0);
          open1();
        }}
        className="flex gap-1 items-center bg-blue-500 w-fit m-auto px-4 py-2 rounded-md text-white font-semibold text-sm -mb-1"
      >
        <Plus className="w-5 h-5" />
        <span>New Student</span>
      </button>
    </div>
  );
};

export default AdminStudents;
