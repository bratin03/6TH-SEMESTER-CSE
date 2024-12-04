import { Link, Navigate } from "react-router-dom";

const Signup = () => {
  if (localStorage.getItem("access_token")) {
    return <Navigate to="/dashboard" replace />;
  }

  return (
    <div className="h-screen flex items-center justify-center bg-gradient-to-br from-blue-800 via bg-blue-500 to-blue-300">
      <div className="bg-white rounded-xl px-8 py-6 flex flex-col gap-4 w-[350px] sm:w-[400px]">
        <div className="flex flex-col">
          <span className="text-2xl font-semibold">Welcome</span>
          <span className="text-sm text-neutral-500 font-semibold">
            Choose what best describes you
          </span>
        </div>

        <div className="flex gap-4 flex-col">
          <Link
            to="/signup/guest-student"
            className="flex flex-col border-2 px-3 py-1 border-white hover:border-2 hover:border-blue-500 rounded-xl transition duration-200"
          >
            <span className="font-semibold text-lg -mb-1">Guest Student</span>
            <span className="text-sm text-neutral-500 font-normal mb-1">
              You are not from IIT KGP but are participating.
            </span>
          </Link>
          <Link
            to="/signup/native-student"
            className="flex flex-col border-2 px-3 py-1 border-white hover:border-2 hover:border-blue-500 rounded-xl transition duration-200"
          >
            <span className="font-semibold text-lg -mb-1">Native Student</span>
            <span className="text-sm text-neutral-500 font-normal mb-1">
              You are a student of IIT KGP.
            </span>
          </Link>
          <Link
            to="/signup/organizer"
            className="flex flex-col border-2 px-3 py-1 border-white hover:border-2 hover:border-blue-500 rounded-xl transition duration-200"
          >
            <span className="font-semibold text-lg -mb-1">Organizer</span>
            <span className="text-sm text-neutral-500 font-normal mb-1">
              You are an organizer of the event.
            </span>
          </Link>
        </div>

        <Link
          to="/login"
          className="text-sm hover:underline text-center w-fit m-auto"
        >
          Already have an account?
        </Link>
      </div>
    </div>
  );
};

export default Signup;
