import { Input, Modal, Select, Table, TextInput, Tooltip } from "@mantine/core";
import { DateInput } from "@mantine/dates";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { format, set } from "date-fns";
import { Info, Pen, Plus, Trash2 } from "lucide-react";
import { useEffect, useState } from "react";
import { toast } from "sonner";
import Button from "./button";

const AdminOrganisers = (props) => {
  const [data, setData] = useState();
  const [search, setSearch] = useState("");
  const [modalData, setModalData] = useState();
  const [selectModal, setSelectModal] = useState(false);

  const [loading, setLoading] = useState(false);

  useEffect(() => {
    if (!props.organiserdata) return;
    setData(props.organiserdata);
  }, [search, props.organiserdata]);

  const [opened1, { open: open1, close: close1 }] = useDisclosure();
  const [opened2, { open: open2, close: close2 }] = useDisclosure();
  const [opened3, { open: open3, close: close3 }] = useDisclosure();

  const form = useForm({
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
        selectModal === 1 || value.length > 5 ? null : "Atleast 6 characters",
    },
  });

  const dateTimeFormatter = (dateTime) => {
    return format(dateTime, "do MMM yyyy, h:mm a");
  };

  const onDelete = (oid) => {
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/remove_organiser/` + oid, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then(() => {
        close2();
        setData((prev) => prev.filter((organiser) => organiser.oid !== oid));
        toast.success("Organiser Deleted");
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  const onSubmit = (values) => {
    form.validate();
    if (Object.keys(form.errors).length > 0) {
      return;
    }

    setLoading(true);

    const url =
      selectModal === 1 ? "/admin/update_organiser" : "/admin/add_organiser";
    fetch(`${process.env.REACT_APP_FETCH_URL}${url}`, {
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
              phone: values.phone,
              password: values.password,
            }
          : {
              oid: modalData.oid,
              email: values.email,
              name: values.name,
              phone: values.phone,
            }
      ),
    })
      .then((response) => response.json())
      .then(() => {
        close3();
        form.reset();
        if (selectModal === 0) {
          setData((prev) => [
            ...prev,
            {
              email: values.email,
              name: values.name,
              phone: values.phone,
              password: values.password,
            },
          ]);
        } else {
          setData((prev) =>
            prev.map((organiser) => {
              if (organiser.oid === modalData.oid) {
                return {
                  ...organiser,
                  name: values.name,
                  phone: values.phone,
                  email: values.email,
                };
              }
              return organiser;
            })
          );
        }
        toast.success(
          selectModal === 0 ? "Organiser Added" : "Organiser Updated"
        );
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  return (
    <div className="px-4 py-1 flex flex-col gap-6">
      <div className="flex justify-start gap-4 items-center">
        <span className="font-semibold text-3xl">Organisers</span>{" "}
        <Modal
          centered
          opened={opened3}
          onClose={close3}
          title={selectModal === 0 ? "New organiser" : "Update organiser"}
        >
          <form onSubmit={form.onSubmit((values) => onSubmit(values))}>
            <TextInput
              label="Email"
              placeholder="Email"
              {...form.getInputProps("email")}
            />
            <TextInput
              label="Name"
              placeholder="Name"
              {...form.getInputProps("name")}
            />

            <TextInput
              label="Phone"
              placeholder="Phone"
              {...form.getInputProps("phone")}
            />
            <TextInput
              label="Password"
              placeholder="Password"
              disabled={selectModal === 1}
              {...form.getInputProps("password")}
            />
            <Button
              type="Submit"
              className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
              text="Submit"
              loading={loading}
            />
          </form>
        </Modal>
        <Modal
          centered
          title="Confirm Delete"
          opened={opened2}
          onClose={close2}
        >
          <p>Do you want to delete this organiser?</p>
          <Button
            onClick={() => onDelete(modalData.oid)}
            className="bg-red-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
            text="Delete"
            loading={loading}
          />
        </Modal>
        <Modal
          centered
          title="Sponsored Events"
          opened={opened1}
          onClose={close1}
        >
          {
            /* iterate on organisr.events_sponsered using loop and print */

            <div>
              {/* make a tabe to display  */}
              <Table striped highlightOnHover withTableBorder>
                <Table.Thead>
                  <Table.Tr>
                    <Table.Th>Event Name</Table.Th>
                    <Table.Th>Sponsor Status</Table.Th>
                  </Table.Tr>
                </Table.Thead>
                <Table.Tbody>
                  {modalData?.events_sponsored &&
                    modalData.events_sponsored.map((event1) => (
                      <Table.Tr key={event1.eid}>
                        <Table.Td>{event1.name}</Table.Td>
                        <Table.Td>{event1.sponsor_status}</Table.Td>
                      </Table.Tr>
                    ))}
                </Table.Tbody>
              </Table>
            </div>
          }
        </Modal>
      </div>
      <Table striped highlightOnHover withTableBorder>
        <Table.Thead>
          <Table.Tr>
            <Table.Th>Name</Table.Th>
            <Table.Th>Email</Table.Th>
            <Table.Th>Phone</Table.Th>
            <Table.Th>Sponsered Events</Table.Th>
            <Table.Th>Update</Table.Th>
            <Table.Th>Delete</Table.Th>
          </Table.Tr>
        </Table.Thead>
        <Table.Tbody>
          {data &&
            data.map((organiser) => (
              <Table.Tr key={organiser.oid}>
                <Table.Td>{organiser.name}</Table.Td>
                <Table.Td>{organiser.email}</Table.Td>
                <Table.Td>{organiser.phone}</Table.Td>

                <Table.Td>
                  <Info
                    onClick={() => {
                      setModalData(organiser);
                      open1();
                    }}
                    className="w-4 h-4"
                  />
                </Table.Td>

                <Table.Td>
                  <Pen
                    onClick={() => {
                      setSelectModal(1);
                      setModalData(organiser);
                      form.setValues({
                        email: organiser.email,
                        name: organiser.name,
                        phone: organiser.phone,
                      });
                      open3();
                    }}
                    className="w-5 h-5"
                  />
                </Table.Td>

                <Table.Td>
                  <Trash2
                    onClick={() => {
                      setModalData(organiser);
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
          setSelectModal(0);
          open3();
        }}
        className="flex gap-1 items-center bg-blue-500 w-fit m-auto px-4 py-2 rounded-md text-white font-semibold text-sm -mb-1"
      >
        <Plus className="w-5 h-5" />
        <span>New Organizer</span>
      </button>
    </div>
  );
};

export default AdminOrganisers;
