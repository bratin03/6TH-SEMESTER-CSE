import {
  Input,
  Modal,
  NumberInput,
  Select,
  Table,
  TextInput,
  Textarea,
  Tooltip,
} from "@mantine/core";
import { DateInput, DateTimePicker, DatesProvider } from "@mantine/dates";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { format, set } from "date-fns";
import { Info, Pen, Plus, Trash2 } from "lucide-react";
import { useEffect, useState } from "react";
import { toast } from "sonner";
import "dayjs/locale/en-in";
import Button from "./button";

const AdminEvents = ({ data, setData }) => {
  const [opened1, { open: open1, close: close1 }] = useDisclosure();
  const [opened2, { open: open2, close: close2 }] = useDisclosure();
  const [opened3, { open: open3, close: close3 }] = useDisclosure();
  const [modalData, setModalData] = useState();
  const [selectModal, setSelectModal] = useState(); // for selecting add or update modal

  const [from, setFrom] = useState(null);
  const [to, setTo] = useState(null);
  const [fromError, setFromError] = useState("");
  const [toError, setToError] = useState("");

  const [loading, setLoading] = useState(false);

  const form = useForm({
    initialValues: {
      name: "",
      type: "",
      location: "",
      first_prize: 0,
      second_prize: 0,
      third_prize: 0,
      info: "",
    },
    validate: {
      name: (value) => (value.length > 0 ? null : "Name is required"),
      type: (value) => (value.length > 0 ? null : "Type is required"),
      location: (value) => (value.length > 0 ? null : "Location is required"),
      first_prize: (value, values) =>
        value >= 0 ? null : "First Prize is required",
      second_prize: (value, values) =>
        value >= 0
          ? values.type !== "competition" || value < values.first_prize
            ? null
            : "Second prize must be less than first"
          : "Second Prize is required",
      third_prize: (value, values) =>
        value >= 0
          ? values.type !== "competition" || value < values.second_prize
            ? null
            : "Third price must be less than second"
          : "Third Prize is required",
      info: (value) => (value.length > 0 ? null : "Info is required"),
    },
  });

  const dateTimeFormatter = (dateTime) => {
    return format(dateTime, "do MMM yyyy, h:mm a");
  };

  const onDelete = (eid) => {
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/delete_event/` + eid, {
      method: "DELETE",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then(() => {
        setData((prev) => prev.filter((event) => event.eid !== eid));
        close2();
        toast.success("Event deleted");
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
  };

  const dateString = (dateTime) => {
    return format(dateTime, "yyyy-MM-dd HH:mm:ss");
  };

  const onSubmit = (values) => {
    form.validate();
    if (
      Object.keys(form.errors).length === 0 &&
      from &&
      to &&
      from <= to &&
      from >= new Date()
    ) {
      setLoading(true);
      from.setHours(from.getHours() - 5);
      from.setMinutes(from.getMinutes() - 30);
      to.setHours(to.getHours() - 5);
      to.setMinutes(to.getMinutes() - 30);
      const fetchUrl =
        selectModal === 0
          ? `${process.env.REACT_APP_FETCH_URL}/admin/add_event`
          : `${process.env.REACT_APP_FETCH_URL}/admin/update_event/${modalData.eid}`;
      fetch(fetchUrl, {
        method: selectModal === 0 ? "POST" : "PUT",
        headers: {
          "Content-Type": "application/json",
          Authorization: "Bearer " + localStorage.getItem("access_token"),
        },
        body: JSON.stringify({
          name: values.name,
          type: values.type,
          start_date_time: dateString(from),
          end_date_time: dateString(to),
          location: values.location,
          first_prize:
            values.type === "competition" ? values.first_prize : "None",
          second_prize:
            values.type === "competition" ? values.second_prize : "None",
          third_prize:
            values.type === "competition" ? values.third_prize : "None",
          info: values.info,
        }),
      })
        .then((response) => response.json())
        .then((resData) => {
          close3();
          form.reset();
          setFrom(null);
          setTo(null);
          from.setHours(from.getHours() + 5);
          from.setMinutes(from.getMinutes() + 30);
          to.setHours(to.getHours() + 5);
          to.setMinutes(to.getMinutes() + 30);
          if (selectModal === 0)
            setData((prev) => [
              ...prev,
              {
                eid: resData.event_id,
                name: values.name,
                type: values.type,
                start_date_time: dateString(from),
                end_date_time: dateString(to),
                location: values.location,
                first_prize:
                  values.type === "competition" ? values.first_prize : null,
                second_prize:
                  values.type === "competition" ? values.second_prize : null,
                third_prize:
                  values.type === "competition" ? values.third_prize : null,
                info: values.info,
              },
            ]);
          else
            setData((prev) =>
              prev.map((event) =>
                event.eid === modalData.eid
                  ? {
                      eid: modalData.eid,
                      name: values.name,
                      type: values.type,
                      start_date_time: from,
                      end_date_time: to,
                      location: values.location,
                      first_prize:
                        values.type === "competition"
                          ? values.first_prize
                          : null,
                      second_prize:
                        values.type === "competition"
                          ? values.second_prize
                          : null,
                      third_prize:
                        values.type === "competition"
                          ? values.third_prize
                          : null,
                      info: values.info,
                    }
                  : event
              )
            );
          toast.success(selectModal === 0 ? "Event Added" : "Event Updated");
          setLoading(false);
        })
        .catch((e) => {
          toast.error(e.message);
          setLoading(false);
        });
    }
    if (!from) {
      setFromError("From date required");
      return;
    } else setFromError("");
    if (!to) {
      setToError("To date required");
      return;
    } else if (from > to) {
      setToError("To date should be after than from date");
      return;
    } else if (from < new Date()) {
      setFromError("From date should be after current date");
      return;
    } else setToError("");
    if (Object.keys(form.errors).length > 0) {
      return;
    }
    if (fromError.length === 0 && toError.length === 0) {
      setFromError("");
      setToError("");
    }
  };

  return (
    <div className="px-4 py-1 flex flex-col gap-6">
      <div className="flex justify-start gap-4 items-center">
        <span className="font-semibold text-3xl">Admin Events</span>

        <Modal centered title="Information" opened={opened1} onClose={close1}>
          {modalData && modalData.info}
        </Modal>

        <Modal
          centered
          title="Confirm Delete"
          opened={opened2}
          onClose={close2}
        >
          <p>Do you want to delete this event?</p>
          <p>{modalData && modalData.name}</p>
          <Button
            onClick={() => onDelete(modalData.eid)}
            className="bg-red-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
            text="Delete"
            loading={loading}
          />
        </Modal>

        <Modal
          centered
          opened={opened3}
          onClose={close3}
          title={selectModal === 0 ? "New Event" : "Update Event"}
        >
          <form onSubmit={form.onSubmit((values) => onSubmit(values))}>
            <TextInput
              label="Name"
              placeholder="Name"
              {...form.getInputProps("name")}
            />
            <Select
              mt="sm"
              label="Type"
              placeholder="Type"
              data={[
                { label: "Competition", value: "competition" },
                { label: "Cultural", value: "cultural" },
                { label: "Workshop", value: "workshop" },
                { label: "Talk", value: "talk" },
                { label: "Other", value: "other" },
              ]}
              {...form.getInputProps("type")}
            />

            <div className="flex gap-4 w-full">
              <div className="w-full">
                <DateTimePicker
                  mt="md"
                  label="From"
                  placeholder="From"
                  value={from}
                  onChange={setFrom}
                  error={fromError.length !== 0}
                />
                <span className="text-xs text-red-500">{fromError}</span>
              </div>

              <div className="w-full">
                <DateTimePicker
                  mt="md"
                  label="To"
                  placeholder="Till"
                  value={to}
                  onChange={setTo}
                  error={toError.length !== 0}
                />
                <span className="text-xs text-red-500">{toError}</span>
              </div>
            </div>

            <TextInput
              mt="sm"
              label="Location"
              placeholder="Location"
              {...form.getInputProps("location")}
            />
            <NumberInput
              mt="sm"
              disabled={form.values.type !== "competition"}
              label="First Prize"
              placeholder="First Prize"
              {...form.getInputProps("first_prize")}
            />
            <NumberInput
              mt="sm"
              disabled={form.values.type !== "competition"}
              label="Second Prize"
              placeholder="Second Prize"
              {...form.getInputProps("second_prize")}
            />
            <NumberInput
              mt="sm"
              disabled={form.values.type !== "competition"}
              label="Third Prize"
              placeholder="Third Prize"
              {...form.getInputProps("third_prize")}
            />
            <Textarea
              mt="sm"
              label="Info"
              placeholder="Info"
              {...form.getInputProps("info")}
            />
            <Button
              type="submit"
              className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm mt-6"
              text="Submit"
              loading={loading}
            />
          </form>
        </Modal>
      </div>
      <Table striped highlightOnHover withTableBorder>
        <Table.Thead>
          <Table.Tr>
            <Table.Th>Event Name</Table.Th>
            <Table.Th>Type</Table.Th>
            <Table.Th>Start Date-Time</Table.Th>
            <Table.Th>End Date-Time</Table.Th>
            <Table.Th>Location</Table.Th>
            <Table.Th>First Prize</Table.Th>
            <Table.Th>Second Prize</Table.Th>
            <Table.Th>Third Prize</Table.Th>
            <Table.Th>Info</Table.Th>
            <Table.Th>Update</Table.Th>
            <Table.Th>Delete</Table.Th>
          </Table.Tr>
        </Table.Thead>
        <Table.Tbody>
          {data &&
            data.map((event) => (
              <Table.Tr key={event.eid}>
                <Table.Td>{event.name}</Table.Td>
                <Table.Td className="capitalize">{event.type}</Table.Td>
                <Table.Td>{dateTimeFormatter(event.start_date_time)}</Table.Td>
                <Table.Td>{dateTimeFormatter(event.end_date_time)}</Table.Td>
                <Table.Td>{event.location}</Table.Td>
                <Table.Td>{event.first_prize}</Table.Td>
                <Table.Td>{event.second_prize}</Table.Td>
                <Table.Td>{event.third_prize}</Table.Td>
                <Table.Td>
                  <Info
                    onClick={() => {
                      setModalData(event);
                      open1();
                    }}
                    className="w-4 h-4"
                  />
                </Table.Td>
                <Table.Td>
                  <Pen
                    onClick={() => {
                      setSelectModal(1); //modal opens for update functionality
                      form.setValues({
                        name: event.name,
                        type: event.type,
                        location: event.location,
                        first_prize:
                          event.type === "competition" ? event.first_prize : 0,
                        second_prize:
                          event.type === "competition" ? event.second_prize : 0,
                        third_prize:
                          event.type === "competition" ? event.third_prize : 0,
                        info: event.info,
                      });
                      setFrom(new Date(event.start_date_time));
                      setTo(new Date(event.end_date_time));
                      setModalData(event);
                      open3();
                    }}
                    className="cursor-pointer w-5 h-5"
                  />
                </Table.Td>
                <Table.Td>
                  <Trash2
                    onClick={() => {
                      setModalData(event);
                      open2();
                    }}
                    className="cursor-pointer w-5 h-5 text-red-600"
                  />
                </Table.Td>
              </Table.Tr>
            ))}
        </Table.Tbody>
      </Table>
      <button
        onClick={() => {
          setSelectModal(0); //modal opens for add functionality
          open3();
        }}
        className="flex gap-1 items-center bg-blue-500 w-fit m-auto px-4 py-2 rounded-md text-white font-semibold text-sm -mb-1"
      >
        <Plus className="w-5 h-5" />
        <span>New Event</span>
      </button>
    </div>
  );
};

export default AdminEvents;
