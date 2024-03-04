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
import { ArrowLeft, Info, Pen, Plus, Trash2 } from "lucide-react";
import { useEffect, useState } from "react";
import { useNavigate } from "react-router-dom";
import { toast } from "sonner";
import Button from "./button";

const Profile = ({ type, data, backButton, setTab }) => {
  const [currentData, setCurrentData] = useState();
  const [search, setSearch] = useState("");
  const [modalData, setModalData] = useState();
  const [selectModal, setSelectModal] = useState();
  const [password, setPassword] = useState("");

  const [loading, setLoading] = useState(false);

  const navigate = useNavigate();

  useEffect(() => {
    if (!data) return;
    setCurrentData(data);
  }, [search, data]);

  const [opened1, { open: open1, close: close1 }] = useDisclosure();
  const [opened2, { open: open2, close: close2 }] = useDisclosure();
  const [opened3, { open: open3, close: close3 }] = useDisclosure();

  const form_student = useForm({
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
          values.password.length === 0 || values.password.length > 5
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

  const form_organiser = useForm({
    initialValues: {
      email: "",
      name: "",
      phone: "",
      password: "",
    },
    validate: {
      email: (value) => (/^\S+@\S+$/.test(value) ? null : "Invalid email"),
      name: (value) => (value.length > 0 ? null : "Name is required"),
      phone: (value) =>
        value.length > 0
          ? /^\d{10}$/g.test(value)
            ? null
            : "Invalid Phone Number"
          : "Phone number is required",
      password: (value) =>
        value.length === 0 || value.length > 5 ? null : "Atleast 6 characters",
    },
  });

  const onSubmit = (values) => {
    setLoading(true);
    const url =
      selectModal === 0
        ? `${process.env.REACT_APP_FETCH_URL}/student/edit_student`
        : `${process.env.REACT_APP_FETCH_URL}/organiser/edit_organiser`;
    fetch(url, {
      method: "PUT",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
      body: JSON.stringify(
        selectModal === 0
          ? {
              sid: modalData.sid,
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
              oid: modalData.oid,
              email: values.email,
              name: values.name,
              phone: values.phone,
              password: values.password,
            }
      ),
    })
      .then((response) => response.json())
      .then((resData) => {
        selectModal == 0 ? form_student.reset() : form_organiser.reset();
        if (selectModal === 0) {
          setCurrentData((prev) => ({ ...prev, ...values }));
        } else {
          setCurrentData((prev) => ({ ...prev, ...values }));
        }
        toast.success(
          selectModal === 0
            ? "Student Profile Updated"
            : "Organiser Profile Updated"
        );
        setLoading(false);
        selectModal === 0 ? close1() : close2();
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  const onDelete = () => {
    setLoading(true);
    let status;
    const url =
      type === "student"
        ? `${process.env.REACT_APP_FETCH_URL}/student/delete_student`
        : `${process.env.REACT_APP_FETCH_URL}/organiser/delete_organiser`;
    fetch(url, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
      body: JSON.stringify({
        password: password,
      }),
    })
      .then((response) => {
        status = response.status;
        return response.json();
      })
      .then((resData) => {
        close3();

        if (status !== 200) {
          toast.error(resData.message);
          setPassword("");
          return;
        }

        toast.success(
          type === "student"
            ? "Student Profile Deleted"
            : "Organiser Profile Deleted"
        );
        setLoading(false);
        localStorage.removeItem("access_token");
        localStorage.removeItem("tab");
        navigate("/login");
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  return (
    <div className="px-4 py-1">
      {backButton && (
        <button
          className=" mb-4 pr-4 pl-3 py-2 rounded-md border-2 font-semibold text-sm mt-1 flex gap-1 items-center"
          onClick={() => {
            setTab(4);
            localStorage.setItem("tab", 4);
          }}
        >
          <ArrowLeft className="w-4 h-4" />
          Back
        </button>
      )}
      {type && (
        <div className="flex flex-col">
          <div className="flex justify-start gap-4 items-center">
            <span className="font-semibold text-3xl">Profile</span>{" "}
            <Modal
              centered
              opened={opened2}
              onClose={close2}
              title={
                selectModal === 0
                  ? "Update Student Profile"
                  : "Update Organiser Profile"
              }
            >
              <form
                onSubmit={form_organiser.onSubmit((values) => onSubmit(values))}
              >
                <TextInput
                  label="Email"
                  disabled
                  placeholder="Email"
                  {...form_organiser.getInputProps("email")}
                />
                <TextInput
                  label="Name"
                  placeholder="Name"
                  {...form_organiser.getInputProps("name")}
                />

                <TextInput
                  label="Phone"
                  placeholder="Phone"
                  {...form_organiser.getInputProps("phone")}
                />
                <TextInput
                  label="Password"
                  placeholder="Password"
                  type="password"
                  {...form_organiser.getInputProps("password")}
                />
                <Button
                  type="submit"
                  className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
                  text="Submit"
                  loading={loading}
                />
              </form>
            </Modal>
            <Modal
              centered
              opened={opened1}
              onClose={close1}
              title={
                selectModal === 0
                  ? "Update Student Profile"
                  : "Update Organiser Profile"
              }
            >
              <form
                className="gap-2 flex flex-col"
                onSubmit={form_student.onSubmit((values) => onSubmit(values))}
              >
                <TextInput
                  label="Email"
                  disabled
                  placeholder="Email"
                  {...form_student.getInputProps("email")}
                />
                <TextInput
                  label="Name"
                  placeholder="Name"
                  {...form_student.getInputProps("name")}
                />
                <TextInput
                  label="Password"
                  placeholder="Password"
                  type="password"
                  {...form_student.getInputProps("password")}
                />

                <div className="flex gap-4">
                  <TextInput
                    className="w-full"
                    label="Roll Number"
                    placeholder="21CS30006"
                    {...form_student.getInputProps("roll_number")}
                  />
                  <TextInput
                    className="w-full"
                    label="Phone"
                    placeholder="10 digit phone number"
                    {...form_student.getInputProps("phone")}
                  />
                </div>

                <TextInput
                  label="College"
                  placeholder="College"
                  {...form_student.getInputProps("college")}
                />
                <Select
                  clearable
                  placeholder="Department"
                  label="Department"
                  {...form_student.getInputProps("department")}
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
                  {...form_student.getInputProps("year")}
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
                  disabled
                  placeholder="Native or Guest"
                  label="Student Type"
                  {...form_student.getInputProps("type")}
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
          <span className="text-sm font-semibold text-neutral-500">
            {`${type === "student" ? "Student" : "Organizer"} Profile`}
          </span>
        </div>
      )}

      {currentData && (
        <div className="flex flex-col gap-6 w-full md:max-w-[800px] mt-8">
          <div className="flex gap-4 items-center">
            <span className="font-semibold w-[170px]">Name</span>
            <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
              {currentData.name}
            </div>
          </div>
          <div className="flex gap-4 items-center">
            <span className="font-semibold w-[170px]">Email</span>
            <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
              {currentData.email}
            </div>
          </div>
          <div className="flex gap-4 items-center">
            <span className="font-semibold w-[170px]">Phone</span>
            <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
              {currentData.phone}
            </div>
          </div>
          {type !== "organiser" && (
            <div className="flex flex-col gap-6">
              <hr />
              <div className="flex gap-4 items-center">
                <span className="font-semibold w-[170px]">Roll Number</span>
                <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
                  {currentData.roll_number}
                </div>
              </div>
              <div className="flex gap-4 items-center">
                <span className="font-semibold w-[170px]">College</span>
                <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
                  {currentData.college}
                </div>
              </div>
              <div className="flex gap-4 items-center">
                <span className="font-semibold w-[170px]">Department</span>
                <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
                  {currentData.department}
                </div>
              </div>
              <div className="flex gap-4 items-center">
                <span className="font-semibold w-[170px]">Year</span>
                <div className="px-5 py-3 border border-neutral-300 rounded-lg text-sm w-full">
                  {currentData.year}
                </div>
              </div>
            </div>
          )}
        </div>
      )}

      {!backButton && (
        <div className="flex gap-4 mt-4 justify-center w-full">
          <button
            onClick={() => {
              type == "organiser" ? setSelectModal(1) : setSelectModal(0);
              type == "organiser"
                ? form_organiser.setValues({
                    email: currentData.email,
                    name: currentData.name,
                    phone: currentData.phone,
                  })
                : form_student.setValues({
                    college: currentData.college,
                    department: currentData.department,
                    email: currentData.email,
                    name: currentData.name,
                    phone: currentData.phone,
                    roll_number: currentData.roll_number,
                    type: currentData.type,
                    year: currentData.year.toString(),
                    // print
                  });
              setModalData(currentData);
              type == "organiser" ? open2() : open1();
            }}
            className="bg-blue-500 w-fit px-4 py-2 rounded-md text-white font-semibold text-sm"
          >
            Update Profile
          </button>
          <button
            onClick={open3}
            className="border border-red-500 text-red-500 text-sm font-semibold px-4 py-2 rounded-md"
          >
            Delete Profile
          </button>
          <Modal
            centered
            opened={opened3}
            onClose={close3}
            title="Account Deletion"
          >
            <TextInput
              label="Password"
              placeholder="Enter password to confirm account deletion"
              type="password"
              value={password}
              onChange={(e) => setPassword(e.target.value)}
            />
            <Button
              onClick={onDelete}
              className="w-full bg-red-500 text-white font-semibold px-4 py-2 rounded-md mt-4 text-sm"
              text="Delete Account"
              loading={loading}
            />
          </Modal>
        </div>
      )}
    </div>
  );
};

export default Profile;
