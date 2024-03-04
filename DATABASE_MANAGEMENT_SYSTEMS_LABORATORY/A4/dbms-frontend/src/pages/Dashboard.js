import { AppShell, Burger } from "@mantine/core";
import { useDisclosure } from "@mantine/hooks";
import Logo from "../components/Logo";
import {
  BedDouble,
  BellRing,
  CalendarFold,
  CircleUserRound,
  TicketCheck,
  User,
  User2,
} from "lucide-react";
import { useEffect, useState } from "react";
import Schedule from "../components/Schedule";
import Accomodation from "../components/Accomodation";
import Events from "../components/Events";
import Profile from "../components/Profile";
import Event from "../components/Event";
import { Navigate, useNavigate } from "react-router-dom";
import { toast } from "sonner";
import AdminAccodomations from "../components/AdminAccomodations";
import AdminStudents from "../components/AdminStudents";
import AdminEvents from "../components/AdminEvents";
import AdminOrganisers from "../components/AdminOrganisers";
import AdminNotif from "../components/AdminNotif";
import { TailSpin } from "react-loader-spinner";

const Dashboard = () => {
  const [type, setType] = useState("");
  const [studentType, setStudentType] = useState("");
  const [data, setData] = useState();
  const [dataViewProfile, setDataViewProfile] = useState();
  const [events, setEvents] = useState();
  const [studentdata, setstudentdata] = useState();
  const [organiserdata, setorganiserdata] = useState();
  const [notifData, setNotifData] = useState();
  const [opened, { toggle }] = useDisclosure();
  const [tab, setTab] = useState();
  const [eid, setEid] = useState(() => {
    var eventId = localStorage.getItem("eid");
    return eventId;
  });

  const [loading, setLoading] = useState({
    fetch1: false,
    fetch2: false,
    fetch3: false,
    fetch4: false,
    fetch5: false,
  });

  useEffect(() => {
    if (!type) return;
    var prevtab = localStorage.getItem("tab");
    if (type === "admin") saveTab(prevtab ? prevtab : 5);
    else saveTab(prevtab ? prevtab : 0);
  }, [type]);

  // fetch profile
  useEffect(() => {
    fetch(`${process.env.REACT_APP_FETCH_URL}/profile`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        setData(data);
        if (data.hasOwnProperty("sid")) {
          setType("student");
          if (data.type === "external") setStudentType("external");
          else setStudentType("internal");
        } else if (data.hasOwnProperty("oid")) {
          setType("organiser");
        } else {
          setType("admin");
          setTab(5);
        }
        setLoading((prev) => ({ ...prev, fetch1: true }));
      })
      .catch((e) => {
        toast.error(e.message);
      });
  }, []);

  // fetch student data for admin
  useEffect(() => {
    if (type != "admin") {
      setLoading((prev) => ({ ...prev, fetch2: true }));
      return;
    }
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/all_students`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        setstudentdata(data);
        setTab(5);
        localStorage.setItem("tab", 5);
        setLoading((prev) => ({ ...prev, fetch2: true }));
      });
  }, [type]);

  //set organisers for admin
  useEffect(() => {
    if (type != "admin") {
      setLoading((prev) => ({ ...prev, fetch3: true }));
      return;
    }
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/all_organisers`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        setorganiserdata(data);
        setLoading((prev) => ({ ...prev, fetch3: true }));
      });
  }, [type]);

  // fetch events
  useEffect(() => {
    if (!type) return;
    var base_url = process.env.REACT_APP_FETCH_URL;
    if (type === "student" || type === "organiser") base_url += "/event";
    else base_url += "/admin/events";
    fetch(base_url, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: "Bearer " + localStorage.getItem("access_token"),
      },
    })
      .then((response) => response.json())
      .then((data) => {
        setEvents(data);
        setLoading((prev) => ({ ...prev, fetch4: true }));
      })
      .catch((e) => {
        toast.error(e.message);
      });
  }, [type]);

  // fetch notifications for admin
  useEffect(() => {
    if (type !== "admin") {
      setLoading((prev) => ({ ...prev, fetch5: true }));
      return;
    }
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/notifs`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${localStorage.getItem("access_token")}`,
      },
    })
      .then((res) => res.json())
      .then((resData) => {
        setNotifData(resData);
        setLoading((prev) => ({ ...prev, fetch5: true }));
      });
  }, [type]);

  const saveTab = (tab) => {
    localStorage.setItem("tab", tab);
    setTab(parseInt(tab));
  };

  const navigate = useNavigate();
  useEffect(() => {
  }, [tab]);

  if (!localStorage.getItem("access_token")) {
    return <Navigate to="/login" replace />;
  }

  return (
    <AppShell
      header={{ height: 60 }}
      navbar={{
        width: 300,
        breakpoint: "sm",
        collapsed: { mobile: !opened },
      }}
      padding="md"
    >
      <AppShell.Header className="flex gap-4 items-center justify-between pl-4">
        <div className="flex gap-4 items-center">
          <Burger opened={opened} onClick={toggle} hiddenFrom="sm" size="sm" />
          <Logo />
        </div>
        <button
          onClick={() => {
            localStorage.removeItem("access_token");
            localStorage.removeItem("tab");
            localStorage.removeItem("eid");
            navigate("/login");
            toast.success("Logged out");
          }}
          className="border-2 px-4 py-2 rounded-md font-semibold text-sm mr-4"
        >
          Logout
        </button>
      </AppShell.Header>

      <AppShell.Navbar p="md">
        {loading.fetch1 &&
        loading.fetch2 &&
        loading.fetch3 &&
        loading.fetch4 &&
        loading.fetch5 ? (
          <ul className="mt-1">
            {type !== "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 0 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(0);
                  toggle();
                }}
              >
                <TicketCheck className="w-6 h-6" />
                <span className="font-semibold text-md">Events</span>
              </li>
            )}
            {type !== "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 1 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(1);
                  toggle();
                }}
              >
                <CalendarFold className="w-6 h-6" />
                <span className="font-semibold text-md">Schedule</span>
              </li>
            )}
            {studentType === "external" && type === "student" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 2 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(2);
                  toggle();
                }}
              >
                <BedDouble className="w-6 h-6" />
                <span className="font-semibold text-md">
                  Food and Accomodation
                </span>
              </li>
            )}
            {type !== "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 3 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(3);
                  toggle();
                }}
              >
                <CircleUserRound className="w-6 h-6" />
                <span className="font-semibold text-md">Profile</span>
              </li>
            )}

            {/* admin tabs */}
            {type == "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 5 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(5);
                  toggle();
                }}
              >
                <TicketCheck className="w-6 h-6" />
                <span className="font-semibold text-md">Events</span>
              </li>
            )}
            {type == "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 7 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(7);
                  toggle();
                }}
              >
                <CircleUserRound className="w-6 h-6" />
                <span className="font-semibold text-md">Students</span>
              </li>
            )}
            {type == "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 8 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(8);
                  toggle();
                }}
              >
                <User className="w-6 h-6" />
                <span className="font-semibold text-md">Organisers</span>
              </li>
            )}
            {type == "admin" && (
              <li
                className={`flex gap-4 items-center mb-8 px-4 py-2 rounded-full cursor-pointer duration-300 ${
                  tab === 10 ? "bg-blue-200 text-blue-600" : ""
                }`}
                onClick={() => {
                  saveTab(10);
                  toggle();
                }}
              >
                <BellRing className="w-5 h-5" />
                <span className="font-semibold text-md">Notifications</span>
              </li>
            )}
          </ul>
        ) : (
          <div className="w-full h-full flex justify-center">
            <TailSpin
              height={60}
              width={60}
              color="black"
              ariaLabel="loading"
              className=""
            />
          </div>
        )}
      </AppShell.Navbar>

      {/* {tab !== undefined && ( */}
      <AppShell.Main>
        {!loading.fetch1 ||
        !loading.fetch2 ||
        !loading.fetch3 ||
        !loading.fetch4 ||
        !loading.fetch5 ? (
          <div className="w-full h-full flex justify-center">
            <TailSpin
              height={60}
              width={60}
              color="black"
              ariaLabel="loading"
              className=""
            />
          </div>
        ) : tab === 0 ? (
          <Events setTab={setTab} setEid={setEid} type={type} events={events} />
        ) : tab === 1 ? (
          <Schedule events={events} type={type} />
        ) : tab === 2 ? (
          <Accomodation />
        ) : tab === 3 ? (
          <Profile type={type} data={data} />
        ) : tab === 4 ? (
          <Event
            eid={eid}
            sid={data?.sid}
            setTab={setTab}
            setEventsData={setEvents}
            setDataViewProfile={setDataViewProfile}
          />
        ) : tab === 5 ? (
          <AdminEvents setTab={setTab} data={events} setData={setEvents} />
        ) : tab === 7 ? (
          <AdminStudents setTab={setTab} studentdata={studentdata} />
        ) : tab === 8 ? (
          <AdminOrganisers setTab={setTab} organiserdata={organiserdata} />
        ) : tab === 9 ? (
          <Profile
            setTab={setTab}
            backButton={1}
            type="student"
            data={dataViewProfile}
          />
        ) : tab === 10 ? (
          <AdminNotif data={notifData} setData={setNotifData} />
        ) : (
          <span>
            bruh {tab} {typeof tab}
          </span>
        )}
      </AppShell.Main>
      {/* )} */}
    </AppShell>
  );
};

export default Dashboard;
