import { set } from "date-fns";
import { IndianRupee } from "lucide-react";
import { useEffect, useState } from "react";
import { toast } from "sonner";
import Button from "./button";

const AdminNotif = ({ data, setData }) => {
  const [loading, setLoading] = useState(false);

  useEffect(() => {
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/notifs`, {
      method: "GET",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${localStorage.getItem("access_token")}`,
      },
    })
      .then((res) => res.json())
      .then((resData) => {
        setData(resData);
      });
  }, []);

  const onApprove = (notif) => {
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/approve_organiser`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${localStorage.getItem("access_token")}`,
      },
      body: JSON.stringify({ oid: notif.organiser_id, eid: notif.event_id }),
    })
      .then((res) => res.json())
      .then((resData) => {
        setData(data.filter((d) => d !== notif));
        toast.success(resData.message);
        setLoading(false);
      });
  };

  const onReject = (notif) => {
    setLoading(true);
    fetch(`${process.env.REACT_APP_FETCH_URL}/admin/reject_organiser`, {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
        Authorization: `Bearer ${localStorage.getItem("access_token")}`,
      },
      body: JSON.stringify({ oid: notif.organiser_id, eid: notif.event_id }),
    })
      .then((res) => res.json())
      .then((resData) => {
        setData(data.filter((d) => d !== notif));
        toast.success(resData.message);
        setLoading(false);
      });
  };

  return (
    <div className="px-4 py-1 flex flex-col gap-6">
      <div className="flex justify-start gap-4 items-center">
        <span className="font-semibold text-3xl">Notifications</span>
      </div>
      {data?.map((notif) => (
        <div className="px-4 py-2 rounded-md shadow-lg border flex flex-row flex-wrap gap-2 items-center w-fit">
          <div className="flex gap-2 items-center mr-4">
            <div className="flex flex-col w-fit">
              <span className="text-md font-semibold">{notif.oname}</span>
              <span className="text-xs font-semibold text-neutral-500">
                {notif.email}
              </span>
            </div>
            <div className="w-[1px] border h-8" />
            <div className="flex gap-x-1 gap-y-0 flex-wrap items-center">
              <span className="text-sm">wants to sponsor</span>
              <span className="font-semibold text-lg">{notif.ename}</span>
              <span className="text-sm">with amount</span>
              <span className="flex items-center">
                <IndianRupee className="w-3 h-3 mt-0.5" />
                {notif.sponsorship_amount}
              </span>
            </div>
          </div>
          <div className="flex gap-4">
            {/* <div className="w-[1px] border h-8 mx-2" /> */}
            <Button
              onClick={() => onApprove(notif)}
              className="bg-blue-500 text-white text-sm font-semibold px-4 py-1.5 rounded-md"
              text="Approve"
            />
            <Button
              onClick={() => onReject(notif)}
              className="border-red-500 border text-red-500 text-sm font-semibold px-4 py-1.5 rounded-md"
              text="Reject"
            />
          </div>
        </div>
      ))}
      {data?.length === 0 && (
        <div className="flex justify-center items-center h-32">
          <span className="text-lg font-semibold text-neutral-500">
            No notifications
          </span>
        </div>
      )}
    </div>
  );
};
export default AdminNotif;
