import React from "react";
import { BrowserRouter as Router, Route, Routes } from "react-router-dom";
import Login from "./pages/Login";
import "@mantine/core/styles.css";
import "@mantine/dates/styles.css";
import { MantineProvider } from "@mantine/core";
import Signup from "./pages/Signup";
import SignupStudent from "./pages/SignupStudent";
import SignupOrganizer from "./pages/SignupOrganizer";
import Dashboard from "./pages/Dashboard";
import { Toaster } from "sonner";
import Landing from "./pages/Landing";

export default function App() {
  return (
    <MantineProvider>
      <Router>
        <Routes>
          <Route path="/login" element={<Login />} />
          <Route
            path="/signup/guest-student"
            element={<SignupStudent type="guest" />}
          />
          <Route
            path="/signup/native-student"
            element={<SignupStudent type="native" />}
          />
          <Route path="/signup/organizer" element={<SignupOrganizer />} />
          <Route path="/signup" element={<Signup />} />
          <Route path="/dashboard" element={<Dashboard />} />
          <Route path="/" element={<Landing />} />
        </Routes>
      </Router>
      <Toaster richColors closeButton />
    </MantineProvider>
  );
}
