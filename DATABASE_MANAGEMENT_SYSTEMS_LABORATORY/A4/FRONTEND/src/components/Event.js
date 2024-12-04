import {
  Input,
  Modal,
  NumberInput,
  Pill,
  Radio,
  Select,
  Table,
  Textarea,
} from "@mantine/core";
import { useForm } from "@mantine/form";
import { useDisclosure } from "@mantine/hooks";
import { format, set } from "date-fns";
import { ArrowLeft, Pen, IndianRupee } from "lucide-react";
import { useEffect, useState } from "react";
import { toast } from "sonner";
import Confetti from "./confetti";
import { TailSpin } from "react-loader-spinner";
import Button from "./button";

const Event = (props) => {
  const [data, setData] = useState();
  const [radio, setRadio] = useState(() => {
    if (localStorage.getItem("radio") === null) return 0;
    else return parseInt(localStorage.getItem("radio"));
  });
  const [opened1, { open: open1, close: close1 }] = useDisclosure();
  const [opened2, { open: open2, close: close2 }] = useDisclosure();
  const [opened3, { open: open3, close: close3 }] = useDisclosure();

  const [currentDateTime, setCurrentDateTime] = useState(new Date());

  const [loading, setLoading] = useState(false);
  const [loading2, setLoading2] = useState(false);

  useEffect(() => {
    // Update the current date and time every second
    const intervalId = setInterval(() => {
      setCurrentDateTime(new Date());
    }, 1000);

    // Cleanup the interval when the component is unmounted
    return () => clearInterval(intervalId);
  }, []);

  const dateTimeFormatter = (dateTime) => {
    return format(dateTime, "do MMM yyyy, h:mm a");
  };

  const form1 = useForm({
    initialValues: {
      role: "",
    },
    validate: {
      role: (value) => (value.length > 0 ? null : "Role required"),
    },
  });

  const form2 = useForm({
    initialValues: {
      sponsorship_amount: 0,
    },
    validate: {
      sponsorship_amount: (value) => (value > 0 ? null : "Amount required"),
    },
  });

  const form3 = useForm({
    initialValues: {
      first_winner: data?.first_winner || "",
      second_winner: data?.second_winner || "",
      third_winner: data?.third_winner || "",
    },
    validate: {
      first_winner: (value) =>
        value.length > 0 ? null : "First winner required",
      second_winner: (value, values) =>
        value.length > 0
          ? value != values.first_winner && value != values.third_winner
            ? null
            : "Second winner cannot be same as first winner"
          : "Second winner required",
      third_winner: (value, values) =>
        value.length > 0
          ? value != values.second_prize && value != values.first_winner
            ? null
            : "Third winner cannot be same as second winner"
          : "Third winner required",
    },
  });

  useEffect(() => {
    if (!data?.first_winner) return;
    form3.setValues({
      first_winner: data.first_winner.sid,
      second_winner: data.second_winner.sid,
      third_winner: data.third_winner.sid,
    });
  }, [data]);

  const onClick1 = () => {
    //register
    setLoading(true);
    let status;
    fetch(
      `${process.env.REACT_APP_FETCH_URL}/event/register_student/${props.eid}`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: "Bearer " + localStorage.getItem("access_token"),
        },
      }
    )
      .then((response) => {
        status = response.status;
        response.json();
      })
      .then((resData) => {
        if (status !== 200) {
          toast.error(resData.message);
          setLoading(false);
        } else {
          setData((prev) => {
            return { ...prev, registered: true };
          });
          props.setEventsData((prev) =>
            prev.map((event) => {
              if (event.eid !== data.eid) return event;
              else return { ...event, registered: true };
            })
          );
          toast.success("Registered for event!");
          setLoading(false);
        }
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
    close1();
  };

  const onClick2 = () => {
    //volunteer
    setLoading(true);
    let status;
    fetch(
      `${process.env.REACT_APP_FETCH_URL}/event/volunteer_student/${props.eid}`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: "Bearer " + localStorage.getItem("access_token"),
        },
        body: JSON.stringify({
          role: form1.values.role,
          info: getRoleInfo(form1.values.role),
        }),
      }
    )
      .then((response) => {
        status = response.status;
        response.json();
      })
      .then((resData) => {
        if (status !== 200) {
          toast.error(resData.message);
          setLoading(false);
        } else {
          setData((prev) => {
            return { ...prev, volunteered: true };
          });
          props.setEventsData((prev) =>
            prev.map((event) => {
              if (event.eid !== data.eid) return event;
              else return { ...event, registered: true };
            })
          );
          toast.success("Volunteering for event!");
          setLoading(false);
        }
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
    close2();
  };

  const onClick3 = () => {
    // Sponsor
    setLoading(true);
    let status;
    fetch(`${process.env.REACT_APP_FETCH_URL}/event/sponsor/${props.eid}`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
      body: JSON.stringify({
        sponsorship_amount: form2.values.sponsorship_amount,
      }),
    })
      .then((response) => {
        status = response.status;
        response.json();
      })
      .then((resData) => {
        if (status !== 200) {
          toast.error(resData.message);
          setLoading(false);
        } else {
          setData((prev) => {
            return { ...prev, sponsored: "pending" };
          });
          props.setEventsData((prev) =>
            prev.map((event) => {
              if (event.eid !== data.eid) return event;
              else return { ...event, sponsored: "pending" };
            })
          );
          toast.info("Sponsorship request sent to admin.");
          setLoading(false);
        }
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading(false);
      });
    close3();
    form2.reset();
  };

  const onSubmit = (values) => {
    setLoading2(true);
    fetch(
      `${process.env.REACT_APP_FETCH_URL}/organiser/set_winners/${props.eid}`,
      {
        method: "POST",
        headers: {
          "Content-Type": "application/json",
          Authorization: "Bearer " + localStorage.getItem("access_token"),
        },
        body: JSON.stringify({
          sid1: values.first_winner,
          sid2: values.second_winner,
          sid3: values.third_winner,
        }),
      }
    )
      .then((response) => response.json())
      .then((resData) => {
        setData((prev) => ({
          ...prev,
          first_winner: values.first_winner,
          second_winner: values.second_winner,
          third_winner: values.third_winner,
        }));
        toast.success("Winners declared!");
        setLoading2(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading2(false);
      });
  };

  const getRoleInfo = (role) => {
    if (role === "Operations Manager")
      return "Operations Manager is responsible for the overall operations of the event. They are responsible for the smooth functioning of the event.";
    else if (role === "Logistics Coordinator")
      return "Logistics Coordinator is responsible for the logistics of the event. They are responsible for the transportation and storage of the event materials.";
    return "-";
  };

  //fetch event details
  useEffect(() => {
    if (!props.eid) return;
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/event/${props.eid}`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        setData(data);
        console.log(data);
        setLoading(false);
      })
      .catch((e) => {
        toast.error(e.message);
      });
  }, [props.eid]);

  // fetch student profile from organizer
  const onClick4 = (sid) => {
    setLoading2(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/organiser/student/${sid}`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        props.setDataViewProfile(data);
        localStorage.setItem("tab", 9);
        props.setTab(9);
        setLoading2(false);
      })
      .catch((e) => {
        toast.error(e.message);
        setLoading2(false);
      });
  };

  return (
    <div className="ml-4">
      {props.sid &&
        (props.sid === data?.first_winner?.sid ||
          props.sid === data?.second?.sid ||
          props.sid === data?.third_winner?.sid) && <Confetti />}
      <button
        className="pr-4 pl-3 py-2 rounded-md border-2 font-semibold text-sm mt-2 flex gap-1 items-center"
        onClick={() => {
          props.setTab(0);
          localStorage.setItem("tab", 0);
        }}
      >
        <ArrowLeft className="w-4 h-4" />
        Back
      </button>
      {data ? (
        <div className="flex flex-col gap-4">
          <div className="flex gap-4 items-center mt-4 mb-6">
            <div className="text-3xl font-semibold">{data.name}</div>
            <Pill className="font-semibold capitalize">{data.type}</Pill>
          </div>
          <div className="flex gap-4 flex-wrap">
            <div className="flex flex-col mr-8">
              <span className="text-sm font-semibold">From</span>
              <span>{data.start_date_time}</span>
            </div>
            <div className="flex flex-col mr-8">
              <span className="text-sm font-semibold">To</span>
              <span>{data.end_date_time}</span>
            </div>
            <div className="flex flex-col mr-8">
              <span className="text-sm font-semibold">Location</span>
              <span>{data.location}</span>
            </div>
          </div>
          <div className="flex gap-4 mt-4">
            {"registered" in data &&
              (currentDateTime < new Date(data.end_date_time) &&
              !data.registered ? (
                <div>
                  <Modal
                    centered
                    opened={opened1}
                    onClose={close1}
                    title="Confirm"
                  >
                    Are you sure you want to register for:
                    <p className="font-medium">
                      {" " + data.name} {"(" + data.type + ")"}
                    </p>
                    <Button
                      onClick={onClick1}
                      className="mt-4 bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
                      text="Confirm"
                      loading={loading}
                    />
                  </Modal>
                  <button
                    onClick={() => {
                      if (data.volunteered)
                        toast.error("You are a volunteer, cannot register!");
                      else open1();
                    }}
                    className="bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
                  >
                    Register
                  </button>
                </div>
              ) : !data.registered ? (
                <div className="border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Event Ended
                </div>
              ) : (
                <div className="border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Already Registered
                </div>
              ))}
            {"volunteered" in data &&
              (currentDateTime < new Date(data.end_date_time) &&
              !data.volunteered ? (
                <div>
                  <Modal
                    centered
                    opened={opened2}
                    onClose={close2}
                    title="Confirm"
                  >
                    <form
                      onSubmit={form1.onSubmit((values) => onClick2(values))}
                    >
                      <Select
                        label="Role"
                        placeholder="Select volunteer role"
                        data={[
                          {
                            value: "Operations Manager",
                            label: "Operations Manager",
                          },
                          {
                            value: "Logistics Coordinator",
                            label: "Logistics Coordinator",
                          },
                        ]}
                        {...form1.getInputProps("role")}
                      />
                      <div className="flex flex-col my-4">
                        <span className="text-sm">Info</span>
                        <span className="bg-neutral-300 rounded-md px-3 py-2 text-sm">
                          {getRoleInfo(form1.values.role)}
                        </span>
                      </div>
                      <Button
                        type="submit"
                        className="mt-4 bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
                        text="Confirm"
                        loading={loading}
                      />
                    </form>
                  </Modal>
                  <button
                    onClick={() => {
                      if (data.registered)
                        toast.error("You are registered, cannot volunteer!");
                      else open2();
                    }}
                    className=" border border-blue-500 px-4 py-2 rounded-md text-blue-500 font-semibold text-sm"
                  >
                    Volunteer
                  </button>
                </div>
              ) : !data.volunteered ? (
                <div className=" border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Event Ended
                </div>
              ) : (
                <div className=" border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Already Volunteered
                </div>
              ))}
            {"sponsored" in data &&
              (currentDateTime < new Date(data.end_date_time) &&
              data.sponsored === "no" ? (
                <div>
                  <Modal
                    centered
                    opened={opened3}
                    onClose={close3}
                    title="Sponsor"
                  >
                    <p className="font-medium">
                      {" " + data.name} {"(" + data.type + ")"}
                    </p>
                    <form
                      className="mt-2"
                      onSubmit={form2.onSubmit((values) => onClick3(values))}
                    >
                      <NumberInput
                        label="Sponsorship Amount"
                        placeholder="Choose amount for the sponsorship"
                        {...form2.getInputProps("sponsorship_amount")}
                      />
                      <Button
                        type="submit"
                        className="mt-4 bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
                        text="Confirm"
                        loading={loading}
                      />
                      {/* <button
                        type="submit"
                        className="mt-4 bg-blue-500 px-4 py-2 rounded-md text-white font-semibold text-sm"
                      >
                        Confirm
                      </button> */}
                    </form>
                  </Modal>
                  <button
                    onClick={open3}
                    className="border border-green-500 px-4 py-2 rounded-md text-green-500 font-semibold text-sm"
                  >
                    Sponsor
                  </button>
                </div>
              ) : data.sponsored === "no" ? (
                <div className="border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Event Ended
                </div>
              ) : data.sponsored === "pending" ? (
                <div className=" border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Sponsorship Request Pending
                </div>
              ) : data.sponsored === "approved" ? (
                <div className=" border px-4 py-2 rounded-md text-neutral-500 font-semibold text-sm">
                  Sponsorship Request Approved
                </div>
              ) : (
                <div className=" border px-4 py-2 rounded-md text-red-500 border-red-500 font-semibold text-sm">
                  Sponsorship Request Rejected
                </div>
              ))}
          </div>
          {data.first_prize && (
            <div className="flex flex-col mt-4 gap-1">
              <span className="text-sm font-semibold">Prizes</span>
              <div className="flex gap-4">
                <span className="bg-yellow-500 px-2 py-1 text-sm rounded-full text-white font-semibold">
                  1st: <IndianRupee className="inline w-4 h-4 ml-1 mb-0.5" />
                  {data.first_prize}
                </span>
                <span className="bg-neutral-300 px-2 py-1 text-sm rounded-full text-neutral-500 font-semibold">
                  2nd: <IndianRupee className="inline w-4 h-4 ml-1 mb-0.5" />
                  {data.second_prize}
                </span>
                <span className="bg-[#b08d57] px-2 py-1 text-sm rounded-full text-white font-semibold">
                  3rd: <IndianRupee className="inline w-4 h-4 ml-1 mb-0.5" />
                  {data.third_prize}
                </span>
              </div>
            </div>
          )}
          {"first_winner" in data && data && (
            <div className="mt-4">
              <span className="font-semibold text-xl">Winners</span>
              <div className="flex gap-16 mt-2">
                <div className="flex flex-col">
                  <span className="border-yellow-500 w-fit text-yellow-500 font-semibold">
                    1st Place
                  </span>
                  <span className="text-lg">{data.first_winner.name}</span>
                  <span className="text-sm">{data.first_winner.email}</span>
                </div>
                <div className="flex flex-col">
                  <span className="border-neutral-300 text-neutral-400 font-semibold w-fit">
                    2nd Place
                  </span>
                  <span className="text-lg">{data.second_winner.name}</span>
                  <span className="text-sm">{data.second_winner.email}</span>
                </div>
                <div className="flex flex-col">
                  <span className="border-[#b08d57] text-[#b08d57] font-semibold w-fit">
                    3rd Place
                  </span>
                  <span className="text-lg">{data.third_winner.name}</span>
                  <span className="text-sm">{data.third_winner.email}</span>
                </div>
              </div>
            </div>
          )}
          {"logistics" in data && data && (
            <div className="mt-2 flex flex-col gap-2">
              <hr />
              <span className="font-semibold text-xl">Declare Winners</span>
              {true && (
                <form onSubmit={form3.onSubmit((values) => onSubmit(values))}>
                  <div className="flex flex-col gap-2 lg:flex-row lg:gap-6 lg:items-end">
                    <Select
                      className="min-w-64"
                      label="First Place Winner"
                      placeholder="Select first place"
                      disabled={data.first_winner}
                      data={data?.participants?.map((participant) => {
                        return {
                          value: participant.sid,
                          name: participant.name,
                          email: participant.email,
                          label: participant.name,
                        };
                      })}
                      renderOption={({ option }) => (
                        <div className="flex flex-col">
                          <span>{option.name}</span>
                          <span className="font-semibold text-xs">
                            {option.email}
                          </span>
                        </div>
                      )}
                      {...form3.getInputProps("first_winner")}
                    />
                    <Select
                      className="min-w-64"
                      label="Second Place Winner"
                      placeholder="Select second place"
                      disabled={data.first_winner}
                      data={data?.participants?.map((participant) => {
                        return {
                          value: participant.sid,
                          name: participant.name,
                          email: participant.email,
                          label: participant.name,
                        };
                      })}
                      renderOption={({ option }) => (
                        <div className="flex flex-col">
                          <span>{option.name}</span>
                          <span className="font-semibold text-xs">
                            {option.email}
                          </span>
                        </div>
                      )}
                      {...form3.getInputProps("second_winner")}
                    />
                    <Select
                      className="min-w-64"
                      label="Third Place Winner"
                      placeholder="Select third place"
                      disabled={data.first_winner}
                      data={data?.participants?.map((participant) => {
                        return {
                          value: participant.sid,
                          name: participant.name,
                          email: participant.email,
                          label: participant.name,
                        };
                      })}
                      renderOption={({ option }) => (
                        <div className="flex flex-col">
                          <span>{option.name}</span>
                          <span className="font-semibold text-xs">
                            {option.email}
                          </span>
                        </div>
                      )}
                      {...form3.getInputProps("third_winner")}
                    />
                    {!data.first_winner && (
                      <Button
                        type="submit"
                        className="bg-blue-500 text-white px-4 py-2 rounded-lg w-fit h-fit mt-3 text-sm font-semibold"
                        text="Declare"
                        loading={loading2}
                      />
                    )}
                  </div>
                </form>
              )}
              <hr />
            </div>
          )}
          {"logistics" in data && data && (
            <div className="mt-4 flex flex-col gap-2">
              <span className="font-semibold text-xl">Event Details</span>
              <div className="flex gap-8">
                {/* <Radio
                  label="Logistics"
                  checked={radio === 0}
                  onChange={() => {
                    setRadio(0);
                    localStorage.setItem("radio", 0);
                  }}
                /> */}
                <Radio
                  label="Participants"
                  checked={radio === 1}
                  onChange={() => {
                    setRadio(1);
                    localStorage.setItem("radio", 1);
                  }}
                />
                <Radio
                  label="Volunteers"
                  checked={radio === 2}
                  onChange={() => {
                    setRadio(2);
                    localStorage.setItem("radio", 2);
                  }}
                />
              </div>

              <Table striped highlightOnHover withTableBorder>
                <Table.Thead>
                  <Table.Tr>
                    {radio === 0 && (
                      <>
                        <Table.Th>Name</Table.Th>
                        <Table.Th>Price</Table.Th>
                        <Table.Th>Quantity</Table.Th>
                      </>
                    )}
                    {radio === 1 && (
                      <>
                        <Table.Th>Profile</Table.Th>
                        <Table.Th>Name</Table.Th>
                        <Table.Th>Email</Table.Th>
                      </>
                    )}
                    {radio === 2 && (
                      <>
                        <Table.Th>Profile</Table.Th>
                        <Table.Th>Name</Table.Th>
                        <Table.Th>Email</Table.Th>
                        <Table.Th>Role</Table.Th>
                        <Table.Th>Info</Table.Th>
                      </>
                    )}
                  </Table.Tr>
                </Table.Thead>
                <Table.Tbody>
                  {radio === 0 &&
                    data.logistics.map((logistic) => (
                      <Table.Tr>
                        <Table.Td>{logistic.name}</Table.Td>
                        <Table.Td>
                          <IndianRupee className="inline w-4 h-4 mb-0.5" />
                          {logistic.price}
                        </Table.Td>
                        <Table.Td>{logistic.quantity}</Table.Td>
                      </Table.Tr>
                    ))}
                  {radio === 1 &&
                    data.participants.map((participant) => (
                      <Table.Tr>
                        <Table.Th onClick={() => onClick4(participant.sid)}>
                          <span className="font-normal hover:underline cursor-pointer">
                            Link
                          </span>
                        </Table.Th>
                        <Table.Td>{participant.name}</Table.Td>
                        <Table.Td>{participant.email}</Table.Td>
                      </Table.Tr>
                    ))}
                  {radio === 2 &&
                    data.volunteers.map((volunteer) => (
                      <Table.Tr>
                        <Table.Th onClick={() => onClick4(volunteer.vid)}>
                          <span className="font-normal hover:underline cursor-pointer">
                            Link
                          </span>
                        </Table.Th>
                        <Table.Td>{volunteer.name}</Table.Td>
                        <Table.Td>{volunteer.email}</Table.Td>
                        <Table.Td>{volunteer.role}</Table.Td>
                        <Table.Td>{volunteer.info}</Table.Td>
                      </Table.Tr>
                    ))}
                </Table.Tbody>
              </Table>
            </div>
          )}
        </div>
      ) : (
        <div className="w-full mt-4 flex justify-center">
          <TailSpin height={60} width={60} color="black" ariaLabel="loading" />
        </div>
      )}
    </div>
  );
};

export default Event;
